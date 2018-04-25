#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/csma-module.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>


using namespace ns3;

void only_udp(NodeContainer *all, Ipv4InterfaceContainer *ip_h3_r2, uint16_t port)
{
    int pckt_size[]={2,4,8,16,32,64,128,256,512,1024};
    for(int i=1; i<=10;i++)
    {
        OnOffHelper Source("ns3::UdpSocketFactory", InetSocketAddress (ip_h3_r2->GetAddress (1), port));

        Source.SetConstantRate(DataRate("100kbps"));
        Source.SetAttribute ("PacketSize", UintegerValue (pckt_size[i-1]));
    //Create source node at n0 (h1)
        ApplicationContainer SourceApp = Source.Install(all->Get(0));

        PacketSinkHelper Sink("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));

        ApplicationContainer SinkApp = Sink.Install(all->Get(4));

        SourceApp.Start (Seconds (0));
        SourceApp.Stop (Seconds (10.0));
        SinkApp.Start (Seconds (0));
        SinkApp.Stop (Seconds (10.0));
        printf("Run Simulation.\n");
        Simulator::Stop (Seconds (22.0));
        Simulator::Run ();
        
        printf("Done.\n");

    }

}

int main()
{

    Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));
    NodeContainer all;
    all.Create(6);

    NodeContainer h1r1 = NodeContainer (all.Get(0), all.Get(2));
    NodeContainer h2r1 = NodeContainer (all.Get(1), all.Get(2));
    NodeContainer h3r2 = NodeContainer (all.Get(3), all.Get(4));
    NodeContainer h4r2 = NodeContainer (all.Get(3), all.Get(5));
    NodeContainer r1r2 = NodeContainer (all.Get(2), all.Get(3));

    InternetStackHelper network;
    network.Install(all);
    
    PointToPointHelper h_r, r_r;

    h_r.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    h_r.SetChannelAttribute("Delay",StringValue("20ms"));

    NetDeviceContainer h1_r1 = h_r.Install(h1r1);
    NetDeviceContainer h2_r1 = h_r.Install(h2r1);
    NetDeviceContainer h3_r2 = h_r.Install(h3r2);
    NetDeviceContainer h4_r2 = h_r.Install(h4r2);

    r_r.SetDeviceAttribute("DataRate", StringValue("30Mbps"));
    r_r.SetChannelAttribute("Delay",StringValue("100ms"));
    r_r.SetQueue("ns3::DropTailQueue","MaxPackets", StringValue("375000"));

    NetDeviceContainer r1_r2 = r_r.Install(r1r2);

    //Assign IP addresses

    Ipv4AddressHelper ip;
    
    ip.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer ip_h1_r1 = ip.Assign(h1_r1);
    ip.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer ip_h2_r1 = ip.Assign(h2_r1);
    ip.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer ip_h3_r2 = ip.Assign(h3_r2);
    ip.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer ip_h4_r2 = ip.Assign(h4_r2);
    ip.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer ip_r1_r2 = ip.Assign(r1_r2);

//     Ptr<Node> node = all.Get (i); // Get pointer to ith node in container
// Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> (); // Get Ipv4 instance of the node
// Ipv4Address addr = ipv4->GetAddress (1, 0).GetLocal (); // Get Ipv4InterfaceAddress of xth interface.
// NS_LOG_INFO(addr);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    uint16_t port = 9;

     

    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    /*for(int pckt_size=1; pckt_size<=1;pckt_size++)
    {
        OnOffHelper Source("ns3::UdpSocketFactory", InetSocketAddress (ip_h3_r2.GetAddress (1), port));

        Source.SetConstantRate(DataRate("100kbps"));
        Source.SetAttribute ("PacketSize", UintegerValue (pckt_size*2048));
    //Create soe node at n0 (h1)
        ApplicationContainer SourceApp = Source.Install (all.Get(0));

        PacketSinkHelper Sink("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));

        ApplicationContainer SinkApp = Sink.Install(all.Get(4));

        SourceApp.Start (Seconds (0));
        SourceApp.Stop (Seconds (10.0));
        SinkApp.Start (Seconds (0));
        SinkApp.Stop (Seconds (10.0));

        printf("Run Simulation.\n");
        Simulator::Stop (Seconds (20.0));
        Simulator::Run ();
        
        printf("Done.\n");
        //std::string name=std::to_string(pckt_size);
        
    }*/
    
    
    only_udp(&all,&ip_h3_r2,port);
    Simulator::Destroy ();
    flowMonitor->SerializeToXmlFile("udp.xml", false,false);
    return 0;
}


