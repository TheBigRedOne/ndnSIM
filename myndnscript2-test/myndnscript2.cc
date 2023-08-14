#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM/helper/ndn-link-control-helper.hpp"

using namespace ns3;

namespace ns3
{
	int main(int argc, char* argv[])
	{
		//Set default parameters
		CommandLine cmd;
		cmd.Parse(argc, argv);

		//Create  nodes
		Ptr<Node> producer = CreateObject<Node>();
		Ptr<Node> consumer = CreateObject<Node>();
		Ptr<Node> router1 = CreateObject<Node>();
		Ptr<Node> router2 = CreateObject<Node>();

		//Create Topology
		PointToPointHelper p2p;
		NetDeviceContainer ndc1 = p2p.Install(producer, router1); //Connect producer and router1
		NetDeviceContainer ndc2 = p2p.Install(producer, router2); //Connect producer and router2
		p2p.Install(consumer, router1); //Connect consumer and router1
		p2p.Install(router1, router2); //Connect router1 and router2

		//Install NDN stack on all nodes
		ns3::ndn::StackHelper ndnHelper;
		ndnHelper.SetDefaultRoutes(true);
		ndnHelper.InstallAll();

		//Choosing routing strategy
		ns3::ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/multicast");

		//Producer
		ns3::ndn::AppHelper producerHelper("ns3::ndn::Producer");
		producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
		producerHelper.SetPrefix("/prefix");
		producerHelper.Install(producer);

		//Consumer
		ns3::ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
		consumerHelper.SetPrefix("/prefix");
		consumerHelper.SetAttribute("Frequency", StringValue("1"));
		consumerHelper.Install(consumer);

		//Enable simulator
		ns3::Simulator::Stop(Seconds(20.0));

		//In inital state, enable the link between producer and router1, diable the link between producer and router2
		ns3::Simulator::Schedule(Seconds(0.0), &ns3::ndn::LinkControlHelper::FailLink, producer, router2);

		//After 10 seconds, disable the link between producer and router1, enable the link between producer and router2
		ns3::Simulator::Schedule(Seconds(10.0), &ns3::ndn::LinkControlHelper::FailLink, producer, router1);
		ns3::Simulator::Schedule(Seconds(10.0), &ns3::ndn::LinkControlHelper::UpLink, producer, router2);

		ns3::Simulator::Run();
		ns3::Simulator::Destroy();

		return 0;

	}
}

int main(int argc, char* argv[])
{
	return ns3::main(argc, argv);
}
