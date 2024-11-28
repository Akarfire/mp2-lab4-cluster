#pragma once
#include "Queue.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>

class CCluster;

typedef void (*OnClasterUpdateFunction)(CCluster*);


struct TClusterReportData
{
	size_t Time;

	size_t TotalProgramCalls;
	size_t TotalProgramsRunning;
	size_t TotalProgramsFinished;

	size_t AllTicksProgramsRunning;
	std::map<unsigned, size_t> AllTicksPerProcessorProgramsRunning;

	float AverageProgramsRunning;
	std::map<unsigned, size_t> PerProcessorTotalPrograms;
	std::map<unsigned, float> PerProcessorAverageLoad;

	friend std::ostream& operator<<(std::ostream& OutStream, TClusterReportData& InReportData)
	{
		OutStream << "Total time: " << InReportData.Time << " ticks;" << std::endl
			<< "Total Program Calls: " << InReportData.TotalProgramCalls << ";" << std::endl
			<< "Total Programs Running: " << InReportData.TotalProgramsRunning << ";" << std::endl
			<< "Total Programs Finished: " << InReportData.TotalProgramsFinished << ";" << std::endl
			<< "Average Programs Running: " << InReportData.AverageProgramsRunning << ";" << std::endl
			<< std::endl <<"Per Processor Stats: " << std::endl << std::endl;

		for (auto Processor : InReportData.PerProcessorTotalPrograms)
		{
			OutStream << Processor.first << " : Total: " << Processor.second << ", " << "Utilization: " << InReportData.PerProcessorAverageLoad[Processor.first] << ";" << std::endl;
		}

		return OutStream;
	}
};


struct TProgramCall
{
	std::string Name;
	size_t RequiredProcessors;
	size_t ExecutionTime;

	size_t TimeCalled;

	TProgramCall(std::string InName = "", size_t InRequiredProcessors = 0, size_t InExecutionTime = 0) : Name(InName), RequiredProcessors(InRequiredProcessors), ExecutionTime(InExecutionTime) {}
};


struct TProgram
{
	std::string Name;
	size_t RequiredProcessorCount;
	std::set<unsigned> OccupiedProcessors;

	size_t ExecutionStartTime;
	size_t MaxExecutionTime;

	TProgram(): RequiredProcessorCount(0), ExecutionStartTime(0), MaxExecutionTime(0) {};

	TProgram(const TProgramCall& InProgramData, size_t StartTime)
	{
		Name = InProgramData.Name;
		ExecutionStartTime = StartTime;
		MaxExecutionTime = InProgramData.ExecutionTime;
		RequiredProcessorCount = InProgramData.RequiredProcessors;
	}

	void AssignProcessor(unsigned InProcessor)
	{
		OccupiedProcessors.insert(InProcessor);
	}
};


class CProcessor
{
	unsigned ProcessorID;

	bool Occupied;
	std::string AssignedProgram;

public:
	CProcessor(unsigned InProcessorID) : ProcessorID(InProcessorID), Occupied(false), AssignedProgram("None") {}

	bool IsOccupied() const { return Occupied; }
	unsigned GetID() const { return ProcessorID; }
	const std::string& GetAssignedProgram() const { return AssignedProgram; }

	void AssignProgram(std::string InProgramName)
	{
		if (IsOccupied())
			throw(std::runtime_error("Trying to assign program to an occupied processor!"));

		AssignedProgram = InProgramName;
		Occupied = true;
	}

	void ProgramFinished()
	{
		AssignedProgram = "None";
		Occupied = false;
	}
};


class CCluster
{
	size_t ProcessorCount;
	std::vector<CProcessor> Processors;

	size_t MaxProgramsStartPerTick;

	std::map<std::string, TProgram> RunningPrograms;
	TQueue<TProgramCall> WaitingProgramCalls;

	size_t CurrentTime;
	size_t MaxTime;

	OnClasterUpdateFunction OnUpdateEvent;

	std::vector<std::string> ThisTickFinishedPrograms;

	TClusterReportData ClusterReportData;

	size_t FreeProcessors = 0;
	size_t QueueAnalysisDepth;

	void Update();

	bool CanExecuteProgram(const TProgramCall& InProgramCall);
	size_t GetTopProgram();
	void StartProgramExecution(const TProgramCall& InProgramCall);
	void FinishProgramExecution(std::string ProgramName);


public:
	CCluster(size_t InMaxTime, size_t InProcessorCount, size_t InQueueAnalysisDepth = 5, size_t InMaxProgramsStartPerTick = 1);

	void Start(OnClasterUpdateFunction InUpdateEvent);

	size_t GetCurrentTime() { return CurrentTime; }

	TClusterReportData& GetReportData();
	float EvaluateWaitingCallScore(size_t Index);

	// For Visualization
	const std::vector<CProcessor>& GetProcessorData() { return Processors; }
	const TQueue<TProgramCall>& GetWaitingProgramCalls() { return WaitingProgramCalls; }
	const std::map<std::string, TProgram> GetRunningPrograms() { return RunningPrograms; }
	const std::vector<std::string>& GetThisTickFinishedPrograms() { return ThisTickFinishedPrograms; }
	

	void CallProgramExecution(TProgramCall InProgramCall);

};