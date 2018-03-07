/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef CUNB_HELPER_H
#define CUNB_HELPER_H

#include "ns3/cunb-phy-helper.h"
#include "ns3/cunb-mac-helper.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/net-device.h"
#include "ns3/cunb-net-device.h"

namespace ns3 {

/**
 * Helps to create CunbNetDevice objects
 *
 * This class can help create a large set of similar CunbNetDevice objects and
 * configure a large set of their attributes during creation.
 */
class CunbHelper
{
public:
  virtual ~CunbHelper ();

  CunbHelper ();

  /**
   * Install CunbNetDevices on a list of nodes
   *
   * \param phy the PHY helper to create PHY objects
   * \param mac the MAC helper to create MAC objects
   * \param c the set of nodes on which a cunb device must be created
   * \returns a device container which contains all the devices created by this
   * method.
   */
  virtual NetDeviceContainer Install (const CunbPhyHelper &phyHelper,
                                      const CunbMacHelper &macHelper,
                                      NodeContainer c) const;

  /**
   * Install CunbNetDevice on a single node
   *
   * \param phy the PHY helper to create PHY objects
   * \param mac the MAC helper to create MAC objects
   * \param node the node on which a lora device must be created
   * \returns a device container which contains all the devices created by this
   * method.
   */
  virtual NetDeviceContainer Install (const CunbPhyHelper &phyHelper,
                                      const CunbMacHelper &macHelper,
                                      Ptr<Node> node) const;

private:
  virtual void EnablePcapInternal (std::string prefix, Ptr<NetDevice> nd, bool promiscuous, bool explicitFilename);
};

} //namespace ns3

#endif /* CUNB_HELPER_H */

