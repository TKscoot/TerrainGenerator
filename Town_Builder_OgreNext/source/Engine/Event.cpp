#include "Event.h"

CEventHandler* CEventHandler::mInstance = nullptr;

bool CEventHandler::frameStarted(const FrameEvent & evt)
{
	for (auto &cb : mFrameStartedCallbacks)
	{
		cb(evt);
	}

	return true;
}

bool CEventHandler::frameEnded(const FrameEvent & evt)
{
	for (auto &cb : mFrameEndedCallbacks)
	{
		cb(evt);
	}

	return true;
}

bool CEventHandler::frameRenderingQueued(const FrameEvent & evt)
{
	for (auto &cb : mFrameRenderingQueuedCallbacks)
	{
		cb(evt);
	}

	return true;
}
