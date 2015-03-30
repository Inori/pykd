#pragma once

#include <kdlib/process.h>

#include "pythreadstate.h"
#include "pyeventhandler.h"

namespace pykd {

struct TargetSystemAdapter {

    static unsigned long getNumberSystems()
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetSystem::getNumber();
    }

    static kdlib::TargetSystemPtr getCurrent()
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetSystem::getCurrent();
    }

    static kdlib::TargetSystemPtr getSystem(unsigned long index)
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetSystem::getByIndex(index);
    }

    static std::wstring getDescription(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.getDescription();
    }

    static bool isDumpAnalyzing(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.isDumpAnalyzing();
    }

    static bool isKernelDebugging(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.isKernelDebugging();
    }

    static bool is64bitSystem(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.is64bitSystem();
    }

    static unsigned long getNumberProcesses(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.getNumberProcesses();
    }

    static kdlib::TargetProcessPtr getProcessByIndex(kdlib::TargetSystem& system, unsigned long index)
    {
        AutoRestorePyState  pystate;
        return system.getProcessByIndex(index);
    }

    static kdlib::TargetProcessPtr getCurrentProcess(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.getCurrentProcess(); 
    }

    static bool isCurrent(kdlib::TargetSystem& system)
    {
        AutoRestorePyState  pystate;
        return system.isCurrent();
    }
};


struct TargetProcessAdapter {

    static kdlib::TargetProcessPtr getCurrent()
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetProcess::getCurrent();
    }

    static kdlib::TargetProcessPtr getProcess(unsigned long index)
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetProcess::getByIndex(index);
    }

    static unsigned long getNumberProcesses()
    {
        AutoRestorePyState  pystate;
        return kdlib::TargetProcess::getNumber();
    }

    static kdlib::PROCESS_ID getSystemId(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getSystemId();
    }

    static kdlib::MEMOFFSET_64 getPebOffset(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getPebOffset();
    }

    static std::wstring getExeName(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getExecutableName();
    }

    static unsigned long getNumberThreads(kdlib::TargetProcess& process )
    {
        AutoRestorePyState  pystate;
        return process.getNumberThreads();
    }

    static kdlib::TargetThreadPtr getThreadByIndex(kdlib::TargetProcess& process, unsigned long index) 
    {
        AutoRestorePyState  pystate;
        return process.getThreadByIndex(index);
    }

    static kdlib::TargetThreadPtr getCurrentThread(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getCurrentThread();
    }

    static unsigned long getNumberBreakpoints(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.getNumberBreakpoints();
    }

    static Breakpoint* getBreakpointByIndex(kdlib::TargetProcess& process, unsigned long index)
    {
        kdlib::BreakpointPtr  bp;

        {
            AutoRestorePyState  pystate;
            bp = process.getBreakpoint(index);
        }

        return new Breakpoint(bp);
    }

    static bool isCurrent(kdlib::TargetProcess& process)
    {
        AutoRestorePyState  pystate;
        return process.isCurrent();
    }
};


struct TargetThreadAdapter {

    static kdlib::THREAD_ID getSystemId(kdlib::TargetThread& thread )
    {
        AutoRestorePyState  pystate;
        return thread.getSystemId();
    }

    static kdlib::MEMOFFSET_64 getTebOffset(kdlib::TargetThread& thread )
    {
        AutoRestorePyState  pystate;
        return thread.getTebOffset();
    }

    static void setCurrent(kdlib::TargetThread& thread)
    {
        AutoRestorePyState  pystate;
        return thread.setCurrent();
    }

    static bool isCurrent(kdlib::TargetThread& thread)
    {
        AutoRestorePyState  pystate;
        return thread.isCurrent();
    }
};

} // pykd namespace

