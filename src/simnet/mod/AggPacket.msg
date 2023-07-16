import Packet;

enum AggPolicy
{
    ATP = 0;
    MTATP = 1;
    INC = 2;
    NOINC = 3;
}

class AggPacket extends Packet
{
    packetType = AGG;
    AggPolicy aggPolicy = NOINC;
    int round;
    int64_t jobId;
    int workerNumber;
    int treeCost;
    int distance;
    // bool resend;
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

class AggUseIncPacket extends AggPacket
{
    unsigned long aggregatorIndex;
    bool collision;
    bool ecn;
    bool overflow;

    aggPolicy = INC;
    int aggCounter;
}

class AggNoIncPacket extends AggPacket
{
    aggPolicy = NOINC;
}

class ATPPacket extends AggUseIncPacket
{
    aggPolicy = ATP;
    byteLength = 16 + 8; // ATP header total size is 58 Bytes
    uint32_t bitmap0;
    uint32_t bitmap1;
    int fanIndegree0;
    int fanIndegree1;
    int switchIdentifier;
}

class MTATPPacket extends AggUseIncPacket
{
    aggPolicy = MTATP;
    byteLength = 16 + 4;
    bool flags[5];
}

