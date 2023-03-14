#ifndef NODE_AGGRGROUPINFO_H_
#define NODE_AGGRGROUPINFO_H_
#include "Packet_m.h"
class AggrGroupInfo {
private:
    int groupid{-1};
    int numberOfChidren{0};
    std::vector<int> children;
    std::map<int, Packet*> packets;
    std::map<int, int> counter;
    std::map<int, int> senderCounter;
    std::unordered_set<int> notAggred;
    std::unordered_set<int> aggred;
    int bufferSize{0};

public:
    explicit AggrGroupInfo(int groupid, int number, int buffer);
    const Packet* getAggrPacket(int seq) const;
    bool isChildrenFull() const;
    const std::vector<int>& getChildren() const {return children;};
    Packet* aggrPacket(int seq, Packet* pk);
    void reset(int seq);
    ~AggrGroupInfo();
    void insertChildNode(int address);
    bool isGroupHasBuffer() const;
    bool isRecordedAggr(int address) const;
    bool isRecordedNotAggr(int address) const;
    void recordNotAggr(int seq);

};

#endif /* NODE_AGGRGROUPINFO_H_ */
