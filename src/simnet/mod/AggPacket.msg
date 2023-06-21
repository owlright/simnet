import Packet;

enum AggPolicy
{
    ATP = 0;
    MTATP = 1;
    SRAGG = 2;
}

class AggPacket extends Packet
{
    packetType = AGG;
    AggPolicy aggPolicy;
    int round;
    int aggregatorIndex;
    int64_t jobId;
    int workerNumber;
    bool overflow;
    bool resend;
    bool collision;
    bool ecn;
    bool isAck;
    // cheating fields
    int64_t workerRecord[] @custom @sizeGetter(getRecordLen)
                           @sizeSetter(setRecordLen)
                           @getter(getRecord)
                           @setter(setRecord);
}

cplusplus(AggPacket) {{
    protected:
        std::vector<int64_t> workerRecord;
    public:
        // expected methods:
        virtual void setRecordLen(size_t size) {
            workerRecord.reserve(size);
        }
        virtual size_t getRecordLen() const {return workerRecord.size();}
        virtual std::vector<int64_t> getRecord() const {return workerRecord;}
        virtual int64_t getRecord(size_t k) {return workerRecord.at(k);}
        virtual void setRecord(size_t k, int64_t worker) {
            EV_WARN << "You should not use this method, but using addRecord instead." << std::endl;
            workerRecord[k] = worker;
        }
        //new methods:
        virtual void addRecord(int64_t worker) {
            workerRecord.push_back(worker);
        }
        virtual void setRecord(std::vector<int64_t>& workers) {
            auto oldLen = getRecordLen();
            for (auto i = 0; i < oldLen; i++) {
                workerRecord[i] = workers.back();
                workers.pop_back();
            }
            while (!workers.empty()) {
                workerRecord.push_back(workers.back());
                workers.pop_back();
            }
        }
}}

cplusplus(AggPacket::copy) {{
    this->workerRecord = other.workerRecord;
}}

class ATPPacket extends AggPacket
{
    aggPolicy = ATP;
    byteLength = 16 + 8; // ATP header total size is 58 Bytes
    uint32_t bitmap0;
    uint32_t bitmap1;
    int fanIndegree0;
    int fanIndegree1;
    int switchIdentifier;
}

class MTATPPacket extends AggPacket
{
    aggPolicy = MTATP;
    byteLength = 16 + 4;
    int timer;
    int aggCounter;
    bool flags[5];
    int treeCost;
}

class SRAggPacket extends AggPacket
{
    aggPolicy = SRAGG;
    int timer;
    int aggCounter;
    int treeCost;
} 