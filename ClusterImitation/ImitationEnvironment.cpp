#include "Cluster.h"
#include <random>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <iomanip>

using namespace std;


float SimulationTactDuration = 0.2f;

size_t ProcessorCount = 32;
size_t Time = 100;
size_t MaxNewProgramsPerTick = 10;
size_t MaxNewProgramStartsPerTick = 5;
size_t QueueAnalyzisDepth = 5;

float ProgramSpawnThreshold = 0.5;
float ProgramRequiredProcessorsMultiplier = 1.f;
float ProgramExecutionTimeMultiplier = 1.f;


void OnClusterUpdated(CCluster* InCluster);

int main()
{
	cout << "Do you want to customize settings ('y' - if yes): ";
	string Input;
	cin >> Input;

	if (Input == "y")
	{
		cout << "ENTER DATA (careful, its not checked!)" << endl;

		cout << "Simulation Time (number of tics, natural): ";
		cin >> Time;
		cout << "Simulation Tick Duration (seconds, positive float): ";
		cin >> SimulationTactDuration;
		cout << "Processor Number (natural): ";
		cin >> ProcessorCount;
		cout << "Max New Program Starts Per Tick (natural): ";
		cin >> MaxNewProgramStartsPerTick;
		cout << "Queue Analysis Depth (natural, 1 for default): ";
		cin >> QueueAnalyzisDepth;

		cout << endl << "Program spawning:" << endl;
		cout << "Program Spawn Threshold (float 0 - 1, 0.5 for default): ";
		cin >> ProgramSpawnThreshold;
		cout << "Max New Program Starts Per Tick (natural): ";
		cin >> MaxNewProgramsPerTick;
		cout << "Program Required Processor Number Multiplier (positive float, 1 for default): ";
		cin >> ProgramRequiredProcessorsMultiplier;
		cout << "Program Execution Time Multiplier(positive float, 1 for default): ";
		cin >> ProgramExecutionTimeMultiplier;
	}

	CCluster Cluster(Time, ProcessorCount, QueueAnalyzisDepth, MaxNewProgramStartsPerTick);

	srand(time(0));

	try
	{
		Cluster.Start(OnClusterUpdated);
	}

	catch (std::exception e)
	{
		std::cout << std::endl << "ERROR: " << e.what() << std::endl;
	}

	std::cout << std::endl << Cluster.GetReportData();
	return 0;
}

void VisualizeCurrentData(CCluster* InCluster)
{
	system("cls");

	const TClusterReportData& ReportData = InCluster->GetReportData();
	const std::vector<CProcessor>& ProcessorData = InCluster->GetProcessorData();
	const TQueue<TProgramCall>& WaitingCalls = InCluster->GetWaitingProgramCalls();
	const std::map<std::string, TProgram> RunningPrograms = InCluster->GetRunningPrograms();
	const std::vector<std::string>& FinishedPrograms = InCluster->GetThisTickFinishedPrograms();

	cout << "Current Time: " << ReportData.Time << endl << endl;

	size_t InLine = 0;
	for (auto& Processor : ProcessorData)
	{
		if (InLine == 3)
		{
			cout << endl << endl;
			InLine = 0;
		}

		cout << "[ PU" << setw(3) << Processor.GetID() << " : " << setw(13) << Processor.GetAssignedProgram() << "]" << setw(5) << " ";
		InLine++;
	}

	cout << endl << endl << "Top 5 WAITING: " << endl << endl;

	for (int i = 0; i < min(size_t(5), WaitingCalls.size()); i++)
	{
		const TProgramCall& Program = WaitingCalls.Check(i);
		cout << setw(13) << Program.Name << " : Req PUs - " << setw(3) << Program.RequiredProcessors << " : Exec Time - " << setw(5) << Program.ExecutionTime << " : Called on time: " << setw(5) << Program.TimeCalled << " : Score - " << setw(7) << InCluster->EvaluateWaitingCallScore(i) << ";" << endl;
	}

	cout << endl << endl << "FINISHED: " << endl << endl;
	for (auto& ProgramName : FinishedPrograms)
	{
		const TProgram& Program = RunningPrograms.at(ProgramName);
		cout << setw(13) << Program.Name << " : Exec Time - " << setw(5) << Program.MaxExecutionTime << ";" << endl;
	}
}

void OnClusterUpdated(CCluster* InCluster)
{
	VisualizeCurrentData(InCluster);

	for (int i = 0; i < MaxNewProgramsPerTick; i++)
	{
		float Rand = float( rand() % 100 ) / 100.0;
		
		if (Rand > ProgramSpawnThreshold)
		{
			TProgramCall ProgramCall;
			ProgramCall.Name = "Program" + std::to_string(InCluster->GetCurrentTime()) + "_" + std::to_string(i);
			ProgramCall.RequiredProcessors = 1 + size_t(9 * pow(Rand, 3)) * ProgramRequiredProcessorsMultiplier;
			ProgramCall.ExecutionTime = 1 + 25 * pow(Rand, 4) * ProgramExecutionTimeMultiplier;

			//std::cout << "Calling Program: " << ProgramCall.Name << ", Required Processors: " << ProgramCall.RequiredProcessors << ", Execution Time: " << ProgramCall.ExecutionTime << std::endl;
			
			InCluster->CallProgramExecution(ProgramCall);
		}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(unsigned(SimulationTactDuration * 1000)));
}

