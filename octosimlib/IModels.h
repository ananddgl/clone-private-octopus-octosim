#pragma once
class SimulationLoop;

class ISimMessage
{
public:
    ISimMessage();
    virtual ~ISimMessage();

    virtual void Reference();
    virtual bool Dereference();

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
    /*
    virtual void TimerExpired(unsigned long long simulationTime) = 0;
    */

private: 
    ITransport * transport;
};

class IDelayDistribution : ISimObject
{
public:
    IDelayDistribution(SimulationLoop * loop);
    virtual ~IDelayDistribution();

    virtual unsigned long long NextDelay() = 0;
};