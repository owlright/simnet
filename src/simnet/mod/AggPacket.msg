import Packet;

enum AggPolicy
{
    NOINC = 0;
    INC = 1;

}

class AggPacket extends Packet
{
    packetType = AGG;
    AggPolicy aggPolicy = NOINC;
    // must contained fields
    unsigned long aggregatorIndex;
    bool collision;
//    bool ecn;
    bool overflow;

    int64_t jobId;
    int64_t aggSeqNumber;
    int workerNumber;
    int treeCost;
    int distance;
    // cheating fields
    int64_t workerRecord[] @custom @sizeGetter(getRecordLen)
                           @sizeSetter(setRecordLen)
                           @getter(getRecord)
                           @setter(setRecord);
    int receivedNumber;
    int aggCounter;
    int64_t PSAddr;
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
