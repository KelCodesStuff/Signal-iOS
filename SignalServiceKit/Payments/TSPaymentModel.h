//
// Copyright 2021 Signal Messenger, LLC
// SPDX-License-Identifier: AGPL-3.0-only
//

#import <SignalServiceKit/BaseModel.h>
#import <SignalServiceKit/TSPaymentModels.h>

NS_ASSUME_NONNULL_BEGIN

@class AciObjC;
@class DBWriteTransaction;
@class MobileCoinPayment;
@class SignalServiceAddress;

// We store payment records separately from interactions.
//
// * Payment records might correspond to transfers to/from exchanges,
//   without an associated interaction.
// * Interactions might be deleted, but we need to maintain records of
//   all payments.
@interface TSPaymentModel : BaseModel

// Incoming, outgoing, etc.
//
// This is inferred from paymentState.
@property (nonatomic, readonly) TSPaymentType paymentType;

@property (nonatomic, readonly) TSPaymentState paymentState;

// This property only applies if paymentState is .incomingFailure
// or .outgoingFailure.
@property (nonatomic, readonly) TSPaymentFailure paymentFailure;

// Might not be set for unverified incoming payments.
@property (nonatomic, readonly, nullable) TSPaymentAmount *paymentAmount;

@property (nonatomic, readonly) uint64_t createdTimestamp;
@property (nonatomic, readonly) NSDate *createdDate;

// This uses ledgerBlockDate if available and createdDate otherwise.
@property (nonatomic, readonly) NSDate *sortDate;

// Optional. The address of the sender/recipient, if any.
//
// We should not treat this value as valid for unverified incoming payments.
@property (nonatomic, readonly, nullable) NSString *addressUuidString;
@property (nonatomic, readonly, nullable) AciObjC *senderOrRecipientAci;

// Optional. Used to construct outgoing notifications.
//           This should only be set for outgoing payments from the device that
//           submitted the payment.
//           We should clear this as soon as sending notification succeeds.
@property (nonatomic, readonly, nullable) NSString *requestUuidString;

@property (nonatomic, readonly, nullable) NSString *memoMessage;

@property (nonatomic, readonly) BOOL isUnread;

// Optional. If set, the unique id of the interaction displayed in chat
// for this payment. If nil, safe to assume no interaction exists and one
// can be created.
@property (nonatomic, readonly, nullable) NSString *interactionUniqueId;

#pragma mark - MobileCoin

// This only applies to mobilecoin.
@property (nonatomic, readonly, nullable) MobileCoinPayment *mobileCoin;

// This only applies to mobilecoin.
// Used by PaymentFinder.
// This value is zero if not set.
@property (nonatomic, readonly) uint64_t mcLedgerBlockIndex;

// Only set for outgoing mobileCoin payments.
// This only applies to mobilecoin.
// Used by PaymentFinder.
@property (nonatomic, readonly, nullable) NSData *mcTransactionData;

// This only applies to mobilecoin.
// Used by PaymentFinder.
@property (nonatomic, readonly, nullable) NSData *mcReceiptData;

#pragma mark -

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (nullable instancetype)initWithCoder:(NSCoder *)coder NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithUniqueId:(NSString *)uniqueId NS_UNAVAILABLE;
- (instancetype)initWithGrdbId:(int64_t)grdbId uniqueId:(NSString *)uniqueId NS_UNAVAILABLE;

- (instancetype)initWithPaymentType:(TSPaymentType)paymentType
                       paymentState:(TSPaymentState)paymentState
                      paymentAmount:(nullable TSPaymentAmount *)paymentAmount
                        createdDate:(NSDate *)createdDate
               senderOrRecipientAci:(nullable AciObjC *)senderOrRecipientAci
                        memoMessage:(nullable NSString *)memoMessage
                           isUnread:(BOOL)isUnread
                interactionUniqueId:(nullable NSString *)interactionUniqueId
                         mobileCoin:(MobileCoinPayment *)mobileCoin NS_DESIGNATED_INITIALIZER;

// --- CODE GENERATION MARKER

// This snippet is generated by /Scripts/sds_codegen/sds_generate.py. Do not manually edit it, instead run
// `sds_codegen.sh`.

// clang-format off

- (instancetype)initWithGrdbId:(int64_t)grdbId
                      uniqueId:(NSString *)uniqueId
               addressUuidString:(nullable NSString *)addressUuidString
                createdTimestamp:(uint64_t)createdTimestamp
             interactionUniqueId:(nullable NSString *)interactionUniqueId
                        isUnread:(BOOL)isUnread
              mcLedgerBlockIndex:(uint64_t)mcLedgerBlockIndex
                   mcReceiptData:(nullable NSData *)mcReceiptData
               mcTransactionData:(nullable NSData *)mcTransactionData
                     memoMessage:(nullable NSString *)memoMessage
                      mobileCoin:(nullable MobileCoinPayment *)mobileCoin
                   paymentAmount:(nullable TSPaymentAmount *)paymentAmount
                  paymentFailure:(TSPaymentFailure)paymentFailure
                    paymentState:(TSPaymentState)paymentState
                     paymentType:(TSPaymentType)paymentType
               requestUuidString:(nullable NSString *)requestUuidString
NS_DESIGNATED_INITIALIZER NS_SWIFT_NAME(init(grdbId:uniqueId:addressUuidString:createdTimestamp:interactionUniqueId:isUnread:mcLedgerBlockIndex:mcReceiptData:mcTransactionData:memoMessage:mobileCoin:paymentAmount:paymentFailure:paymentState:paymentType:requestUuidString:));

// clang-format on

// --- CODE GENERATION MARKER

- (void)updateWithPaymentState:(TSPaymentState)paymentState
                   transaction:(DBWriteTransaction *)transaction NS_SWIFT_NAME(update(paymentState:transaction:));

- (void)updateWithMCLedgerBlockIndex:(uint64_t)ledgerBlockIndex
                         transaction:(DBWriteTransaction *)transaction
    NS_SWIFT_NAME(update(mcLedgerBlockIndex:transaction:));

- (void)updateWithMCLedgerBlockTimestamp:(uint64_t)ledgerBlockTimestamp
                             transaction:(DBWriteTransaction *)transaction
    NS_SWIFT_NAME(update(mcLedgerBlockTimestamp:transaction:));

- (void)updateWithPaymentFailure:(TSPaymentFailure)paymentFailure
                    paymentState:(TSPaymentState)paymentState
                     transaction:(DBWriteTransaction *)transaction
    NS_SWIFT_NAME(update(withPaymentFailure:paymentState:transaction:));

- (void)updateWithPaymentAmount:(TSPaymentAmount *)paymentAmount
                    transaction:(DBWriteTransaction *)transaction NS_SWIFT_NAME(update(withPaymentAmount:transaction:));

- (void)updateWithIsUnread:(BOOL)isUnread transaction:(DBWriteTransaction *)transaction;

- (void)updateWithInteractionUniqueId:(NSString *)interactionUniqueId transaction:(DBWriteTransaction *)transaction;

@end

#pragma mark -

@interface MobileCoinPayment : MTLModel

// This property is only used for transfer in/out flows.
@property (nonatomic, readonly, nullable) NSData *recipientPublicAddressData;

// Optional. Only set for outgoing mobileCoin payments.
@property (nonatomic, readonly, nullable) NSData *transactionData;

// Optional. Set for incoming and outgoing mobileCoin payments.
@property (nonatomic, readonly, nullable) NSData *receiptData;

// Optional. Set for incoming and outgoing mobileCoin payments.
@property (nonatomic, readonly, nullable) NSArray<NSData *> *incomingTransactionPublicKeys;

// The image keys for the TXOs spent in this outgoing MC transaction.
@property (nonatomic, readonly, nullable) NSArray<NSData *> *spentKeyImages;

// The TXOs spent in this outgoing MC transaction.
@property (nonatomic, readonly, nullable) NSArray<NSData *> *outputPublicKeys;

// This value is zero if not set.
@property (nonatomic, readonly) uint64_t ledgerBlockTimestamp;
@property (nonatomic, readonly, nullable) NSDate *ledgerBlockDate;

// This value is zero if not set.
//
// This only applies to mobilecoin.
@property (nonatomic, readonly) uint64_t ledgerBlockIndex;

// Optional. Only set for outgoing mobileCoin payments.
@property (nonatomic, readonly, nullable) TSPaymentAmount *feeAmount;

- (instancetype)initWithRecipientPublicAddressData:(nullable NSData *)recipientPublicAddressData
                                   transactionData:(nullable NSData *)transactionData
                                       receiptData:(nullable NSData *)receiptData
                     incomingTransactionPublicKeys:(nullable NSArray<NSData *> *)incomingTransactionPublicKeys
                                    spentKeyImages:(nullable NSArray<NSData *> *)spentKeyImages
                                  outputPublicKeys:(nullable NSArray<NSData *> *)outputPublicKeys
                              ledgerBlockTimestamp:(uint64_t)ledgerBlockTimestamp
                                  ledgerBlockIndex:(uint64_t)ledgerBlockIndex
                                         feeAmount:(nullable TSPaymentAmount *)feeAmount;

@end

NS_ASSUME_NONNULL_END
