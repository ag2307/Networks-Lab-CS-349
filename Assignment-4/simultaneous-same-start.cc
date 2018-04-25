/*
     Network topology: Dumbbell
    
        h1 (n0)                                        h3 (n4)
             | 80 Mb/s, 20ms                            |  
             |                 30Mb/s, 100ms            | 80 Mb/s, 20ms
                 r1 (n2) ------------------------- r2 (n3)
             |                                          | 80 Mb/s, 20ms
             | 80 Mb/s, 20ms                            |
        h2 (n1)                                       h4 (n5)
    
     - All links are point-to-point links
     - CBR/UDP flows from h1 to h3
     - FTP/TCP flows from h2 to h4
     - UDP Packet size = 1024 bytes, bitrate =  1Mbps
     - DropTail queues 
*/

#include <fstream>
#include <cassert>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Group_40"); // defining the log component with a specific name.

int main (int argc, char *argv[]){

    Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));
    bool tracing = false;
    bool flow_monitor = true;
    std::string prot = "DEFAULT";   
    
    CommandLine cmd;
    cmd.AddValue ("prot", "Transport protocol to use: TcpHighSpeed, "
                    "TcpVegas, TcpScalable", prot);
    cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing);
    cmd.AddValue ("flow_monitor", "Enable flow monitor", flow_monitor);
    cmd.Parse (argc, argv);

    NS_LOG_INFO ("Create nodes.");
    NodeContainer nc;
      
    nc.Create (6); 
    NodeContainer h1_r1 = NodeContainer (nc.Get (1), nc.Get (0));
    NodeContainer h2_r1 = NodeContainer (nc.Get (2), nc.Get (0));
    NodeContainer h3_r2 = NodeContainer (nc.Get (5), nc.Get (3));
    NodeContainer h4_r2 = NodeContainer (nc.Get (5), nc.Get (4));
    NodeContainer r1_r2 = NodeContainer (nc.Get (0), nc.Get (5));

    
    InternetStackHelper internet;
    internet.Install (nc);

    // Create channels between nodes
    NS_LOG_INFO ("Create channels");
    
    PointToPointHelper p2p;

    //node-router connections
    p2p.SetDeviceAttribute ("DataRate", StringValue ("80Mbps"));  //Datarate = 80Mbps
    p2p.SetChannelAttribute ("Delay", StringValue ("20ms")); //Delay = 20ms
    NetDeviceContainer p2p_h1_r1 = p2p.Install (h1_r1); //Install the p2p connection between h1-r1 or n0-n2
    NetDeviceContainer p2p_h2_r1 = p2p.Install (h2_r1); 
    NetDeviceContainer p2p_h3_r2 = p2p.Install (h3_r2); 
    NetDeviceContainer p2p_h4_r2 = p2p.Install (h4_r2); 
    
    //router-router connections
    p2p.SetDeviceAttribute ("DataRate", StringValue ("30Mbps")); //Datarate = 30Mbps
    p2p.SetChannelAttribute ("Delay", StringValue ("100ms")); //Delay = 100ms
    p2p.SetQueue ("ns3::DropTailQueue", "MaxPackets", StringValue ("384000")); // DropTailQueue
    NetDeviceContainer p2p_r1_r2 = p2p.Install (r1_r2); //creating a node r1r2 

    // Bind IP addresses
    NS_LOG_INFO ("IP Addresses: ");
    Ipv4AddressHelper ipv4; 

    ipv4.SetBase ("10.0.1.0", "255.255.255.0");
    Ipv4InterfaceContainer ip_h1_r1 = ipv4.Assign (p2p_h1_r1);

    ipv4.SetBase ("10.0.2.0", "255.255.255.0");
    Ipv4InterfaceContainer ip_h2_r1 = ipv4.Assign (p2p_h2_r1);

    ipv4.SetBase ("10.0.3.0", "255.255.255.0");
    Ipv4InterfaceContainer ip_h3_r2 = ipv4.Assign (p2p_h3_r2);

    ipv4.SetBase ("10.0.4.0", "255.255.255.0");
    Ipv4InterfaceContainer ip_h4_r2 = ipv4.Assign (p2p_h4_r2);

    ipv4.SetBase ("10.0.5.0", "255.255.255.0");
    Ipv4InterfaceContainer ip_r1_r2 = ipv4.Assign (p2p_r1_r2);

    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    NS_LOG_INFO ("Create Applications.");
    
    uint16_t port = 9;

    
    int packet_sizes[]={2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
    int i;
    for(i=0;i<10;i++) {
        BulkSendHelper ftpSource ("ns3::TcpSocketFactory", InetSocketAddress (ip_h4_r2.GetAddress (1), port));
        ftpSource.SetAttribute ("SendSize", UintegerValue (packet_sizes[i]));
        ftpSource.SetAttribute ("MaxBytes", UintegerValue (0)); // Set the amount of data to send in bytes.  Zero is unlimited.
        
        ApplicationContainer ftpSourceApp = ftpSource.Install (nc.Get (2));
        
        
        PacketSinkHelper tcpSink ("ns3::TcpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
        ApplicationContainer ftpSinkApp = tcpSink.Install (nc.Get (4));
    
       
        OnOffHelper udpSource ("ns3::UdpSocketFactory", InetSocketAddress (ip_h3_r2.GetAddress (1), port));
        udpSource.SetConstantRate (DataRate ("100kbps"));
        udpSource.SetAttribute ("PacketSize", UintegerValue (packet_sizes[i]));
        
        ApplicationContainer udpSourceApp = udpSource.Install (nc.Get (1));
    
        
        PacketSinkHelper udpSink ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
        ApplicationContainer udpSinkApp = udpSink.Install (nc.Get (3));
    
        ftpSourceApp.Start (Seconds (0));
        ftpSourceApp.Stop (Seconds (10.0));
        ftpSinkApp.Start (Seconds (0));
        ftpSinkApp.Stop (Seconds (10.0));
    
        udpSourceApp.Start (Seconds (0));
        udpSourceApp.Stop (Seconds (10.0));
        udpSinkApp.Start (Seconds (0));
        udpSinkApp.Stop (Seconds (10.0));
    }

    // Flow monitor
    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    NS_LOG_INFO ("Run Simulation.");
    Simulator::Stop (Seconds (12.0));
    Simulator::Run ();
    Simulator::Destroy ();
    NS_LOG_INFO ("Done.");
    
    flowMonitor->SerializeToXmlFile("same_start.xml", false,false);
}
