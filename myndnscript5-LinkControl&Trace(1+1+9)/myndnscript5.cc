#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/ndnSIM/helper/ndn-link-control-helper.hpp"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM/utils/tracers/ndn-l3-rate-tracer.hpp"

using namespace ns3;

namespace ns3
{
	int main(int argc, char* argv[])
	{
		// Set default parameters
		CommandLine cmd;
		cmd.Parse(argc, argv);

		// Read topology from file
		AnnotatedTopologyReader topologyReader("", 25);
		topologyReader.SetFileName("scratch/myndnscript5.txt"); // Updated file name
		topologyReader.Read();

		// Install NDN stack on all nodes
		ns3::ndn::StackHelper ndnHelper;
		ndnHelper.SetDefaultRoutes(true);
		ndnHelper.InstallAll();

		// Choosing routing strategy
		ns3::ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/multicast");

		// Producer
		ns3::ndn::AppHelper producerHelper("ns3::ndn::Producer");
		producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
		producerHelper.SetPrefix("/prefix");
		producerHelper.Install(Names::Find<Node>("Producer"));

		// Consumer
		ns3::ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
		consumerHelper.SetAttribute("Frequency", StringValue("10"));
		consumerHelper.SetPrefix("/prefix");
		consumerHelper.Install(Names::Find<Node>("Consumer"));

		// Enable Pcap tracing on specific links
        	PointToPointHelper p2p;
        	p2p.EnablePcap("prefix-consumer-router3", Names::Find<Node>("Consumer")->GetDevice(0));
        	p2p.EnablePcap("prefix-consumer-router6", Names::Find<Node>("Consumer")->GetDevice(1));
        	p2p.EnablePcap("prefix-consumer-router9", Names::Find<Node>("Consumer")->GetDevice(2));

    		// Enable L3RateTracer
    		ndn::L3RateTracer::InstallAll("myndnscript5-rate-trace.txt", Seconds(1));

		// In initial state, enable the link between Consumer and Router3, disable the link between Consumer and Router6 and Router9
		ns3::Simulator::Schedule(Seconds(0.0), &ns3::ndn::LinkControlHelper::UpLink, Names::Find<Node>("Consumer"), Names::Find<Node>("Router3"));
		ns3::Simulator::Schedule(Seconds(0.0), &ns3::ndn::LinkControlHelper::FailLink, Names::Find<Node>("Consumer"), Names::Find<Node>("Router6"));
		ns3::Simulator::Schedule(Seconds(0.0), &ns3::ndn::LinkControlHelper::FailLink, Names::Find<Node>("Consumer"), Names::Find<Node>("Router9"));

		// After 5 seconds, enable the link between Consumer and Router6, disable the link between Consumer and Router3 and Router9
		ns3::Simulator::Schedule(Seconds(5.0), &ns3::ndn::LinkControlHelper::UpLink, Names::Find<Node>("Consumer"), Names::Find<Node>("Router6"));
		ns3::Simulator::Schedule(Seconds(5.0), &ns3::ndn::LinkControlHelper::FailLink, Names::Find<Node>("Consumer"), Names::Find<Node>("Router3"));
		ns3::Simulator::Schedule(Seconds(5.0), &ns3::ndn::LinkControlHelper::FailLink, Names::Find<Node>("Consumer"), Names::Find<Node>("Router9"));

		// After 10 seconds, enable the link between Consumer and Router9, disable the link between Consumer and Router3 and Router6
		ns3::Simulator::Schedule(Seconds(10.0), &ns3::ndn::LinkControlHelper::UpLink, Names::Find<Node>("Consumer"), Names::Find<Node>("Router9"));
		ns3::Simulator::Schedule(Seconds(10.0), &ns3::ndn::LinkControlHelper::FailLink, Names::Find<Node>("Consumer"), Names::Find<Node>("Router3"));
		ns3::Simulator::Schedule(Seconds(10.0), &ns3::ndn::LinkControlHelper::FailLink, Names::Find<Node>("Consumer"), Names::Find<Node>("Router6"));

		Simulator::Stop(Seconds(20));

		Simulator::Run();
		Simulator::Destroy();

		return 0;
	}
}

int main(int argc, char* argv[])
{
	return ns3::main(argc, argv);
}
