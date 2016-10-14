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

class PublisherCallback : public PublisherListener
{
public:
  ~PublisherCallback() {};
  void onPublicationMatched(Publisher* pub, MatchingInfo& info)
  {
    printf("I just published\n");
  }
};

int main(int argc, char** argv)
{
  auto node_name = "my_string_talker";

  ParticipantAttributes PParam; //Configuration structure
  PParam.rtps.setName(node_name);
  PParam.rtps.builtin.domainId = 71;
  PParam.rtps.defaultSendPort = 11511;
  PParam.rtps.use_IP6_to_send = true;
  PParam.rtps.builtin.use_SIMPLE_RTPSParticipantDiscoveryProtocol = true;
  PParam.rtps.builtin.use_SIMPLE_EndpointDiscoveryProtocol = true;
  PParam.rtps.builtin.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
  PParam.rtps.builtin.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
  PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
  Participant* participant = Domain::createParticipant(PParam);

  // register types
  StringPubSubType string;
  if (!Domain::registerType(participant, &string))
  {
    printf("Failed to register type!\n");
    return -1;
  }

  PublisherAttributes Wparam; //Configuration structure
  Wparam.topic.topicKind = NO_KEY;
  Wparam.topic.topicDataType = "String";
  Wparam.topic.topicName = "ChatterTopic";
  Wparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
  Wparam.topic.historyQos.depth = 30;
  Wparam.topic.resourceLimitsQos.max_samples = 50;
  Wparam.topic.resourceLimitsQos.allocated_samples = 20;
  Wparam.times.heartbeatPeriod.seconds = 2;
  Wparam.times.heartbeatPeriod.fraction = 200*1000*1000;
  Wparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;

  PublisherCallback listener; //Class that implements callbacks from the publisher
  Publisher* publisher = Domain::createPublisher(participant, Wparam, &listener);
  if (publisher == nullptr)
  {
    printf("Failed to create a publisher\n");
    return -1;
  }

  ::String msg;
  for(auto i = 0; i < 100; i++)
  {
    msg.data("My FastRTPS string");

    publisher->write(&msg);

    std::this_thread::sleep_for( std::chrono::milliseconds(1000) );
  }

  printf("Good bye %s\n", node_name);

  return 0;
}
