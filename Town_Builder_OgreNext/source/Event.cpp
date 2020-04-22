#include "Event.h"

CEvent* CEvent::mInstance = nullptr;

bool CEvent::frameStarted(const FrameEvent & evt)
{
	for (auto &cb : mFrameStartedCallbacks)
	{
		cb(evt);
	}

	return true;
}

bool CEvent::frameEnded(const FrameEvent & evt)
{
	for (auto &cb : mFrameEndedCallbacks)
	{
		cb(evt);
	}

	return true;
}

bool CEvent::frameRenderingQueued(const FrameEvent & evt)
{
	for (auto &cb : mFrameRenderingQueuedCallbacks)
	{
		cb(evt);
	}

	return true;
}
