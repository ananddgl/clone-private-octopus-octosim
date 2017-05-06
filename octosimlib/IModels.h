/*
* Copyright (c) 2017, Private Octopus, Inc.
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Private Octopus, Inc. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#include <stdio.h>
class SimulationLoop;

class ISimMessage
{
public:
    ISimMessage(unsigned int length = 100);
    virtual ~ISimMessage();

    virtual void Reference();
    virtual bool Dereference();
    virtual void Log(FILE* LogFile, bool dropped);

    unsigned int length;

private:
    int referenceCount;
};

class ISimObject
{
public:
    ISimObject(SimulationLoop * loop);
    virtual ~ISimObject();

    virtual void Input(ISimMessage * message) = 0;
    /* The default timer action is to do nothing. */
    virtual void TimerExpired(unsigned long long simulationTime) {}

    SimulationLoop * GetLoop() { return loop; }
    unsigned int object_number;
private:
    SimulationLoop * loop;
};

class ITransport;
class IPath;

class IApplication : public ISimObject
{
public:
    IApplication(SimulationLoop * loop);
    virtual ~IApplication();

    virtual void SetTransport(ITransport * transport)
    {
        this->transport = transport;
    }

    virtual ITransport * GetTransport()
    {
        return transport;
    }

private:
    ITransport * transport;
};

class ITransport : public ISimObject
{
public:
    ITransport(SimulationLoop * loop);
    virtual ~ITransport();

    virtual void SetApplication(IApplication * application) 
    {
        this->application = application;
    }

    virtual IApplication * GetApplication()
    {
        return application;
    }

    virtual void SetPath(IPath * path)
    {
        this->path = path;
    }

    virtual IPath * GetPath()
    {
        return path;
    }

    virtual void ApplicationInput(ISimMessage * message) = 0; /* input from the application, immediate */
    virtual void Input(ISimMessage * message) = 0; /* input from the network */
    virtual void ResetTimer(unsigned long long delay);
    virtual void SetTimer(unsigned long long delay);
    virtual void RttUpdate(unsigned long long transmitTime, unsigned long long arrivalTime);

    int nb_timers_outstanding;
    unsigned long long next_timer;

    unsigned long long rtt;
    unsigned long long rtt_dev;

private:
    IApplication * application;
    IPath * path;
};

class IPath : public ISimObject
{
public:
    IPath(SimulationLoop * loop);
    virtual ~IPath();

    virtual void SetTransport(ITransport * transport)
    {
        this->transport = transport;
    }

    virtual ITransport * GetTransport()
    {
        return transport;
    }

    virtual void Input(ISimMessage * message) = 0; 
    
    virtual void TimerExpired(unsigned long long simulationTime);

private: 
    ITransport * transport;
};

class IDelayDistribution : public ISimObject
{
public:
    IDelayDistribution(SimulationLoop * loop);
    virtual ~IDelayDistribution();

    virtual unsigned long long NextDelay() = 0;
    virtual void Input(ISimMessage * message) override;
};


class ILengthDistribution : public ISimObject
{
public:
    ILengthDistribution(SimulationLoop * loop);
    virtual ~ILengthDistribution();

    virtual unsigned int NextLength() = 0;
    virtual void Input(ISimMessage * message) override;
};