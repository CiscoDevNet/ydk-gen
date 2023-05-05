/*
 * ------------------------------------------------------------------
 * YANG Development Kit
 * Copyright 2017 Cisco Systems. All rights reserved
 *
 *----------------------------------------------
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http:www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *----------------------------------------------
 */
package main

import (
	"bytes"
	"flag"
	"fmt"
	"strconv"
	"strings"
	ping "github.com/CiscoDevNet/ydk-go/ydk/models/cisco_ios_xr/ping_act"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk"
)

// prepareRpc adds RPC input data to pingRpc object
func prepareRpc(pingRpc *ping.Ping) {
	pingRpc.Input.Destination.Destination = "10.0.0.1"
}

// processPingRpc processes data in RPC output object
func processPingRpc(pingRpc *ping.Ping) string {
	// format string for reply header
	response := &pingRpc.Output.PingResponse.Ipv4[0]

	var reply bytes.Buffer
	replyHeader := "Sending 5, 100-byte ICMP Echos to %v, timeout is 2 seconds:"
	fmt.Fprintln(&reply, fmt.Sprintf(replyHeader, response.Destination))

	for _, rply := range response.Replies.Reply {
		fmt.Fprintln(&reply, rply.Result.(string))
	}

	fmt.Fprintf(&reply, 
		fmt.Sprintf("Success rate is %v", response.SuccessRate))
	fmt.Fprintf(&reply,
		fmt.Sprintf("percent (%v/%v),", response.Hits, response.Total))
	fmt.Fprintf(&reply, fmt.Sprintln("round-trip %v/%v/%v ms",
		response.RttMin, response.RttAvg, response.RttMax))

	return reply.String()
}

// main execute main program.
func main() {
	defer func() {
		if r := recover(); r != nil {
			fmt.Println("\nError occured!!\n ", r)
		}
	}()

	// args
	vPtr := flag.Bool("v", false, "Enable verbose")
	devicePtr := flag.String(
		"device",
		"",
		"NETCONF device (ssh://user:password@host:port)")
	flag.Parse()

	// log debug messages if verbose argument specified 
	if *vPtr {
		ydk.EnableLogging(ydk.Info)
	}

	if (*devicePtr == "") {
		panic("Missing device arg see --help for details")
	}

	ydk.YLogDebug(*devicePtr)
	
	denominators := []string{"://", ":", "@", ":"}
	keys := []string {"protocol", "username", "password", "address", "port"}
	device := make(map[string]string)
	
	var split []string
	unprocessed := *devicePtr
	for i := 0; i < 4; i++ {
		if (!strings.Contains(unprocessed, denominators[i])) {
			panic(fmt.Sprintln("Device arg: device must be entered in",
				"ssh://user:password@host:port format"))
		}
		split = strings.SplitN(unprocessed, denominators[i], 2)

		device[keys[i]] = split[0]
		unprocessed = split[1]
	}
	device[keys[4]] = unprocessed

	port, err := strconv.Atoi(device["port"])
	if (err != nil) {
		panic("Device arg: port must be an int")
	}

	// create NETCONF provider
	provider := providers.NetconfServiceProvider{
		Address: device["address"],
		Username: device["username"],
		Password: device["password"],
		Port: port,
		Protocol: device["protocol"]}
	provider.Connect()

	// create CRUD service
	service := services.ExecutorService{}

	// read data from NETCONF device
	pingRpc := ping.Ping{}			// create object
	prepareRpc(&pingRpc)			// add RPC input

	// execute RPC on NETCONF device
	service.ExecuteRpc(&provider, &pingRpc, nil)
	fmt.Println(processPingRpc(&pingRpc))
}

// <rpc-reply xmlns="urn:ietf:params:xml:ns:netconf:base:1.0" message-id="4">
//   <rpc-error>
//     <error-type>application</error-type>
//     <error-tag>operation-failed</error-tag>
//     <error-severity>error</error-severity>
//     <error-message xml:lang="en">'IPv6PingTrace' detected the 'warning' condition 'Bad hostname or IPv6 address or protocol not running'</error-message>
//   </rpc-error>
// </rpc-reply>
