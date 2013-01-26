#ifndef CDATAPACKET_H
#define CDATAPACKET_H
#include <string>
/*
 *
 * UDP_Engine 
 * Copyright (c) 2011 NoLimitsDesigns
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.

 * If there are any concerns or questions about the code, please e-mail smasherprog@gmail.com or visit www.nolimitsdesigns.com
 *

 *
 * Author: Scott Lee
 */
/*!
	Internal_DataPacket should not be used by anyone. It is used internally!!
*/
class Internal_DataPacket{
	Internal_DataPacket(const Internal_DataPacket& pack);
public:

	static void* operator new(size_t sz);
	static void operator delete(void* ptr);

	Internal_DataPacket(): Data(0), TimesSent(0.0f), Size(0), Capacity(0), Id(0), FirstTime(0), LastTime(0) {}
	Internal_DataPacket(Internal_DataPacket&& other) { *this = std::move(other); }// c++11 move operator
	Internal_DataPacket(unsigned int len);
	Internal_DataPacket(char* data, unsigned short len, unsigned int time, unsigned short id);
	Internal_DataPacket& operator=(Internal_DataPacket&& other){// c++11 move operator
		memcpy(this, &other, sizeof(Internal_DataPacket));
		memset(&other, 0, sizeof(Internal_DataPacket));
	}
	Internal_DataPacket& operator=(const Internal_DataPacket& rhs);

	~Internal_DataPacket();
	void free();// used internally by UDP_Engine

	char            *Data;
	unsigned int	Size, Capacity;// the actual length of all the stored data in the packet
	float			TimesSent;
	unsigned short	Id;
	unsigned int    FirstTime, LastTime;


};
/*!
	cDataPacket is the class used to hold information that a user can and should access.
*/
class cDataPacket  {// simple wrapper to make dealing with the packet easy
public:
	Internal_DataPacket Internal_Packet;
	char            *Data;// this is always offset from where the real data starts in the IneternalPacket so that the user can access their data, not my internal data
	unsigned int Length;// length is not the ACTUAL length of the allocated data, but just the length of the data available to you.

	static void* operator new(size_t sz);
	static void operator delete(void* ptr);

	cDataPacket(): Data(0), Length(0) {}//!< create an empty packet.. . .  no memory is allocated
	cDataPacket(cDataPacket&& other): Data(0), Length(0) { *this = std::move(other); }// c++11 move operator

	cDataPacket(Internal_DataPacket& obj);//!< this will consume the Internal_DataPacket type and set up the cDataPacket. Should be used interally only!!
	cDataPacket(unsigned int len);//!< allocate len bytes of space, this function assumes the number of bytes allocated is the number of bytes to send. 
	cDataPacket(char* data, unsigned int len): Data(0), Length(0) { Init(data, len); }//!< allocate len bytes of space and copy data into the packet
	/*!
		Use this to create a packet to ensure the capacity is of a multiple of blocksize so the UDP_Engine will not have to resize the packet. 
		newsize will be the size of the data, but enough space will be allocated to be of a multiple of blocksize. A common use is cDataPacket packet(25, 16); This will reserve 32 bytes of data in the packet. 
		The size the packet expects you to place data inside will be 25
	*/
	cDataPacket(unsigned int newsize, unsigned int blocksize): Data(0), Length(0) { reservetoblocksize(newsize, blocksize); }
	cDataPacket(const cDataPacket& obj): Data(0), Length(0){ operator=(obj); }//!< copy constructor. Will preform a deep copy of all data, internal and otherwise
	cDataPacket &operator=(cDataPacket &&other ){// c++11 move operator
		Data = other.Data; Length = other.Length; Internal_Packet = std::move(other.Internal_Packet);
	}
	cDataPacket &operator=( const cDataPacket &otherpacket );//!< assignment operator. Will preform a deep copy of all internal data
	/*!
		Use this to ensure the packet is of the capacity is of a multiple of blocksize so the UDP_Engine will not have to resize the packet. 
		newsize will be the size of the data, but enough space will be allocated to be of a multiple of blocksize. A common use is cDataPacket packet(25, 16); This will reserve 32 bytes of data in the packet. 
		The size the packet expects you to place data inside will be 25
	*/
	void reservetoblocksize(unsigned int newsize, unsigned int blocksize = 16);//!< Use this in conjunction with creating an empty packet to ensure the capacity is of a multiple of blocksize so the UDP_Engine will not have to resize the packet. newsize will be the size of the data, but enough space will be allocated to be of a multiple of blocksize
	void resizetoblocksize(unsigned int blocksize = 16);//!< this will resize the packet to be aligned on a blocksize boundary
	void Init(char* data, unsigned int len);//!< This will clear any previous data inside the packet, Grow the packet is needed and copy the data inside
	void reserve(unsigned int newcapacity);
	void resize(unsigned int newsize);

	// this is contained in the header of every regular datapacket sent out by the application layer. Every packet that you send out should have an ID field for the type of packet it is.
	//For example, you should have an enum Packet_Types { Set_Position, Get_Position, Send_Chat_Message }  etc. So, you tag a packet with that id and on the other end, that id is used to process the packet.
	//THIS ID BELOW IS NOT!!! THE SAME ID stored in Internal_DataPacket::Id     They are two DIFFERENT IDS!!!@!@!
	unsigned short int GetID() const;
	void SetID(unsigned short int id);
};


#endif 
