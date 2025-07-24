//
// Copyright 2024 Signal Messenger, LLC
// SPDX-License-Identifier: AGPL-3.0-only
//

import Foundation
public import SignalServiceKit

extension AttachmentMultisend {

    public struct Destination {
        public let conversationItem: ConversationItem
        public let thread: TSThread
        // Message bodies are re-generated _per destination_,
        // as mentions must be hydrated based on participants.
        public let messageBody: ValidatedMessageBody?
    }

    public static func prepareForSending(
        _ messageBody: MessageBody?,
        to conversations: [ConversationItem],
        db: SDSDatabaseStorage,
        attachmentValidator: AttachmentContentValidator
    ) async throws -> [Destination] {

        // If the message body has no mentions, we can "hydrate" once across all threads
        // and share it. We only need to re-generate per-thread if there are mentions.
        let canShareMessageBody = !(messageBody?.ranges.hasMentions ?? false)

        struct PreDestination {
            let conversationItem: ConversationItem
            let thread: TSThread
            let messageBody: HydratedMessageBody?
        }

        let preDestinations: [PreDestination] = try await db.awaitableWrite { tx in
            return try conversations.map { conversation in
                guard let thread = conversation.getOrCreateThread(transaction: tx) else {
                    throw OWSAssertionError("Missing thread for conversation")
                }
                let hydratedMessageBody: HydratedMessageBody?
                if canShareMessageBody {
                    // Don't set per-destination message bodies.
                    hydratedMessageBody = nil
                } else {
                    hydratedMessageBody = messageBody?.forForwarding(to: thread, transaction: tx)
                }
                return .init(
                    conversationItem: conversation,
                    thread: thread,
                    messageBody: hydratedMessageBody
                )
            }
        }

        guard !canShareMessageBody else {
            // We only prepare the single shared body.
            let validatedMessageBody: ValidatedMessageBody?
            if let messageBody {
                // AttachmentValidator runs synchronously _and_ opens write transactions
                // internally. We can't block on the write lock in the cooperative thread
                // pool, so bridge out of structured concurrency to run the validation.
                validatedMessageBody = try await withCheckedThrowingContinuation { continuation in
                    DispatchQueue.global().async {
                        continuation.resume(with: Swift.Result(catching: {
                            try attachmentValidator.prepareOversizeTextsIfNeeded(
                                from: ["": messageBody]
                            ).values.first
                        }))
                    }
                }
            } else {
                validatedMessageBody = nil
            }
            return preDestinations.map {
                .init(
                    conversationItem: $0.conversationItem,
                    thread: $0.thread,
                    messageBody: validatedMessageBody
                )
            }
        }

        // Prepare the message body per-thread.
        var destinations = [Destination]()
        for preDestination in preDestinations {
            guard let hydratedMessageBody = preDestination.messageBody else {
                destinations.append(.init(
                    conversationItem: preDestination.conversationItem,
                    thread: preDestination.thread,
                    messageBody: nil
                ))
                continue
            }
            let validatedMessageBody = try await withCheckedThrowingContinuation { continuation in
                DispatchQueue.global().async {
                    continuation.resume(with: Swift.Result(catching: {
                        try attachmentValidator.prepareOversizeTextsIfNeeded(
                            from: ["": hydratedMessageBody.asMessageBodyForForwarding()]
                        ).values.first
                    }))
                }
            }
            destinations.append(.init(
                conversationItem: preDestination.conversationItem,
                thread: preDestination.thread,
                messageBody: validatedMessageBody
            ))
        }
        return destinations
    }
}
