#pragma once
#include "ThreadStruct.h"

extern ApplicationThread* sbInfo = new ApplicationThread();


DWORD WINAPI UpdateApplicationThread(LPVOID lpParameter)
{

	ApplicationThread* threadPointer = (ApplicationThread*)lpParameter;

	double lastFrameTime = glfwGetTime();
	double timeStep = 0.0;
	DWORD sleepTime_ms = 1;


	while (threadPointer->isActive)
	{
		if (threadPointer->isThreadActive)
		{
			double currentTime = glfwGetTime();
			double deltaTime = currentTime - lastFrameTime;
			lastFrameTime = currentTime;

			timeStep += deltaTime;



			
				//threadPointer->manager->Update(deltaTime);
				threadPointer->entityManager->Update(deltaTime);
				threadPointer->physicsEngine->Update(deltaTime);
		


			Sleep(threadPointer->sleepTime);
		}

	}


	return 0;

}

void StartThreadForSoftBody(float time)
{
	
	sbInfo->desiredUpdateTime = time;
	sbInfo->physicsEngine = &PhysicsEngine::GetInstance();
	sbInfo->manager = &ThreadManager::Getinstance();
	sbInfo->entityManager = &EntityManager::GetInstance();
	sbInfo->panelmanager = &PanelManager::GetInstance();
	sbInfo->isActive = true;
	sbInfo->sleepTime = 1;

	sbInfo->threadHandle = CreateThread(
		NULL,						
		0,							
		UpdateApplicationThread,		
		(void*)sbInfo,			
		0,						
		&sbInfo->ThreadId);		

	// SoftBody Thread
}
