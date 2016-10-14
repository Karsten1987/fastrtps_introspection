#include <fastrtps/Domain.h>
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastrtps/attributes/SubscriberAttributes.h>

#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/rtps/builtin/data/WriterProxyData.h>
#include <fastrtps/rtps/common/CDRMessage_t.h>

#include <fastrtps/rtps/reader/RTPSReader.h>
#include <fastrtps/rtps/reader/StatefulReader.h>
#include <fastrtps/rtps/reader/ReaderListener.h>
#include <fastrtps/rtps/builtin/discovery/endpoint/EDPSimple.h>

#include "../idl/stringPubSubTypes.h"

std::once_flag info_flag;

template<class Message_T>
class SubscriberCallback : public SubscriberListener
{
  SampleInfo_t info_;
  Message_T msg_;

public:
  ~SubscriberCallback() {};
  void onSubscriptionMatched(Subscriber* /*sub*/, MatchingInfo& info)
  {
    //std::call_once(info_flag, [&](){info_ = info;});
    printf("Subscriber callback called\n");
  }

  void onNewDataMessage(Subscriber* sub)
  {
    // msg and info are output parameter
    if(sub->takeNextData((void*)&msg_, &info_))
    {
      if(info_.sampleKind == ALIVE)
      {
        // Print your structure data here.
        printf("New message received %s\n", msg_.data().c_str());
        return;
      }
      printf("Failure on fetching value\n");
    }
  }
};

int main(int argc, char** argv)
{
  auto node_name = "my_string_listener";

  ParticipantAttributes PParam; //Configuration structure
  PParam.rtps.setName(node_name);
  PParam.rtps.builtin.domainId = 71;
  PParam.rtps.defaultSendPort = 10043;
  PParam.rtps.builtin.use_SIMPLE_RTPSParticipantDiscoveryProtocol = true;
  PParam.rtps.builtin.use_SIMPLE_EndpointDiscoveryProtocol = true;
  PParam.rtps.builtin.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
  PParam.rtps.builtin.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
  PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
  Participant *participant = Domain::createParticipant(PParam);

  // register types
  StringPubSubType string;
  if (!Domain::registerType(participant, &string))
  {
    printf("Failed to register type!\n");
  }

  SubscriberAttributes Rparam; //Configuration structure
  Rparam.topic.topicKind = NO_KEY;
  Rparam.topic.topicDataType = "String";
  Rparam.topic.topicName = "StringTopic";
  Rparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
  Rparam.topic.historyQos.depth = 30;
  Rparam.topic.resourceLimitsQos.max_samples = 50;
  Rparam.topic.resourceLimitsQos.allocated_samples = 20;
  Rparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;

  SubscriberCallback<String> listener; //Class that implements callbacks from the Subscriber
  Subscriber *subscriber = Domain::createSubscriber(participant, Rparam, &listener);

  std::cin.ignore();
  printf("Good bye %s\n", node_name);
}
