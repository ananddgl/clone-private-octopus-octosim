#pragma once

class ISimMessage
{
public:
    ISimMessage();
    virtual ~ISimMessage();
};

class ISimObject
{
public:
    ISimObject();
    virtual ~ISimObject();

    virtual void Input(ISimMessage * message) = 0;
    virtual void TimerExpired(unsigned long long simulationTime) = 0;
};

class ITransport;

class IApplication : public ISimObject
{
public:
    IApplication();
    virtual ~IApplication();

    virtual void SetTransport(ITransport * transport) = 0;

    virtual void Input(ISimMessage * message) = 0;
    virtual void TimerExpired(unsigned long long simulationTime) = 0;
};

class ITransport
{
public:
    ITransport();
    virtual ~ITransport();

    virtual void SetApplication(IApplication * application) = 0;
    virtual void SetPath(IApplication * application) = 0;

    virtual void ApplicationInput(ISimMessage * message) = 0; /* input from the application, immediate */
    virtual void Input(ISimMessage * message) = 0; /* input from the network */
    virtual void TimerExpired(unsigned long long simulationTime) = 0;
};

class IPath
{
public:
    IPath();
    virtual ~IPath();

    virtual void SetTransport(ITransport * transport) = 0; /* where to deliver messages? */

    virtual void Input(ISimMessage * message) = 0; 
    virtual void TimerExpired(unsigned long long simulationTime) = 0;
};