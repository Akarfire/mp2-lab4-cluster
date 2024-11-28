#include "Queue.h"
#include "Cluster.h"


CCluster::CCluster(size_t InMaxTime, size_t InProcessorCount, size_t InQueueAnalysisDepth, size_t InMaxProgramsStartPerTick)
{
	ProcessorCount = InProcessorCount;
	MaxTime = InMaxTime;
	CurrentTime = 0;

	QueueAnalysisDepth = InQueueAnalysisDepth;
	MaxProgramsStartPerTick = InMaxProgramsStartPerTick;

	FreeProcessors = 0;
	for (int i = 0; i < InProcessorCount; i++)
	{
		Processors.push_back(CProcessor(i));
		ClusterReportData.PerProcessorTotalPrograms[i] = 0;
		ClusterReportData.AllTicksPerProcessorProgramsRunning[i] = 0;
		FreeProcessors++;
	}
}


void CCluster::Start(OnClasterUpdateFunction InUpdateEvent)
{
	OnUpdateEvent = InUpdateEvent;

	while (CurrentTime <= MaxTime)
		Update();
}


void CCluster::Update()
{

	for (int i = 0; i < MaxProgramsStartPerTick; i++)
	{
		if (!WaitingProgramCalls.empty())
		{
			size_t TopProgramID = GetTopProgram();
			const TProgramCall& TopProgram = WaitingProgramCalls.Check(TopProgramID);
			if (CanExecuteProgram(TopProgram))
			{
				StartProgramExecution(TopProgram);
				WaitingProgramCalls.Pop(TopProgramID);
			}
		}
	}

	if (RunningPrograms.size() > 0)
		for (auto& Program : RunningPrograms)
			if ((Program.second.ExecutionStartTime + Program.second.MaxExecutionTime) <= CurrentTime)
				FinishProgramExecution(Program.first);

	for (auto& Processor : Processors)
		if (Processor.IsOccupied())
			ClusterReportData.AllTicksPerProcessorProgramsRunning[Processor.GetID()]++;

	ClusterReportData.AllTicksProgramsRunning += RunningPrograms.size();

	OnUpdateEvent(this);

	for (auto& FinishedProgram : ThisTickFinishedPrograms)
		RunningPrograms.erase(FinishedProgram);

	ThisTickFinishedPrograms.clear();
	
	CurrentTime++;
}


TClusterReportData& CCluster::GetReportData()
{
	ClusterReportData.Time = CurrentTime;
	ClusterReportData.AverageProgramsRunning = float(ClusterReportData.AllTicksProgramsRunning) / CurrentTime;

	for (int i = 0; i < ProcessorCount; i++)
		ClusterReportData.PerProcessorAverageLoad[i] = float(ClusterReportData.AllTicksPerProcessorProgramsRunning[i]) / CurrentTime;

	return ClusterReportData;
}


bool CCluster::CanExecuteProgram(const TProgramCall& InProgramCall)
{
	size_t FoundFreeProcessors = 0;
	for (auto& Pr : Processors)
	{
		FoundFreeProcessors += !Pr.IsOccupied();

		if (FoundFreeProcessors >= InProgramCall.RequiredProcessors)
			break;
	}

	return FoundFreeProcessors >= InProgramCall.RequiredProcessors;
}


size_t CCluster::GetTopProgram()
{
	size_t Index = 0;
	float MaxScore = -1000000000000000000000000000000000.0;

	size_t i = 0;

	while (i < std::min(QueueAnalysisDepth, WaitingProgramCalls.size()))
	{
		float Score = EvaluateWaitingCallScore(i);
		if (Score > MaxScore)
		{
			Index = i;
			MaxScore = Score;
		}

		i++;
	}

	return Index;
}


float CCluster::EvaluateWaitingCallScore(size_t Index)
{
	float OutScore = 0;

	const TProgramCall& Call = WaitingProgramCalls.Check(Index);

	if (Index <= QueueAnalysisDepth)
		OutScore += (QueueAnalysisDepth - Index) * 15;
	OutScore += (CurrentTime - Call.TimeCalled) * 5;

	OutScore -= Call.ExecutionTime * 4;

	if (FreeProcessors < Call.RequiredProcessors)
		OutScore -= Call.RequiredProcessors * 8;
	

	return OutScore;
}


void CCluster::StartProgramExecution(const TProgramCall& InProgramCall)
{
	TProgram NewProgram(InProgramCall, CurrentTime);

	size_t AssignedProcessorCount = 0;
	for (auto& Pr : Processors)
	{
		if (!Pr.IsOccupied())
		{
			NewProgram.AssignProcessor(Pr.GetID());
			Pr.AssignProgram(InProgramCall.Name);
			
			ClusterReportData.PerProcessorTotalPrograms[Pr.GetID()]++;

			AssignedProcessorCount++;
			FreeProcessors--;
		}

		if (AssignedProcessorCount == InProgramCall.RequiredProcessors)
			break;
	}

	if (AssignedProcessorCount != InProgramCall.RequiredProcessors)
		throw(std::runtime_error("Tried to start a progam, without checking first!"));

	RunningPrograms[InProgramCall.Name] = NewProgram;

	ClusterReportData.TotalProgramsRunning++;
}


void CCluster::FinishProgramExecution(std::string ProgramName)
{
	for (auto& Pr : RunningPrograms[ProgramName].OccupiedProcessors)
	{
		Processors[Pr].ProgramFinished();
		FreeProcessors++;
	}

	ThisTickFinishedPrograms.push_back(ProgramName);

	ClusterReportData.TotalProgramsFinished++;
}


void CCluster::CallProgramExecution(TProgramCall InProgramCall)
{
	if (InProgramCall.RequiredProcessors > ProcessorCount)
		throw(std::runtime_error("Calling a program with too many required processors!"));

	if (InProgramCall.RequiredProcessors == 0)
		throw (std::runtime_error("Calling a program with zero required processors!"));

	if (InProgramCall.Name == "")
		throw (std::runtime_error("Calling a program with an empty name"));

	if (InProgramCall.ExecutionTime == 0)
		throw (std::runtime_error("Calling a program with or zero execution time!"));

	InProgramCall.TimeCalled = CurrentTime;
	WaitingProgramCalls.Put(InProgramCall);

	ClusterReportData.TotalProgramCalls++;
}