#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/ndnSIM/helper/ndn-link-control-helper.hpp"
#include "ns3/ndnSIM/utils/tracers/ndn-l3-rate-tracer.hpp"

using namespace ns3;

namespace ns3
{
	int main(int argc, char* argv[])
	{
		//Set default parameters
		CommandLine cmd;
		cmd.Parse(argc, argv);

		//Read topology from file
		AnnotatedTopologyReader topologyReader("", 25);
		topologyReader.SetFileName("scratch/myndnscript4.txt");
		topologyReader.Read();

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
		producerHelper.Install(Names::Find<Node>("Producer"));

		//Consumer
		ns3::ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
		consumerHelper.SetAttribute("Frequency", StringValue("10"));
		consumerHelper.SetPrefix("/prefix");
		consumerHelper.Install(Names::Find<Node>("Consumer"));

		//Install L3RateTracer
		ndn::L3RateTracer::InstallAll("myndnscript4-rate-trace.txt",Seconds(1.0));

		//Enable simulator
		ns3::Simulator::Stop(Seconds(20.0));

		//In initial state, enable the link between producer and router1, diable the link between producer and router3
		ns3::Simulator::Schedule(Seconds(0.0), &ns3::ndn::LinkControlHelper::FailLink, Names::Find<Node>("Producer"), Names::Find<Node>("Router3"));

		//After 10 seconds, disable the link between producer and router1, enable the link between producer and router3
		ns3::Simulator::Schedule(Seconds(10.0), &ns3::ndn::LinkControlHelper::FailLink, Names::Find<Node>("Producer"), Names::Find<Node>("Router1"));
		ns3::Simulator::Schedule(Seconds(10.0), &ns3::ndn::LinkControlHelper::UpLink, Names::Find<Node>("Producer"), Names::Find<Node>("Router3"));

		Simulator::Run();
		Simulator::Destroy();

		return 0;
	}
}

int main(int argc, char* argv[])
{
	return ns3::main(argc, argv);
}
