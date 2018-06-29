#pragma once

#include <iostream>
#include <mutex>
#include "DataStructureDef.h"

class AnalysisEngine
{
private:
	AnalysisEngine(const AnalysisEngine&) = delete;
	AnalysisEngine & operator=(const AnalysisEngine&) = delete;

	static std::unique_ptr<AnalysisEngine> instance;
	static std::once_flag onceFlag;

public:
	AnalysisEngine() = default;

	static AnalysisEngine& Instance()
	{
		std::call_once(AnalysisEngine::onceFlag, []()
		{
			instance.reset(new AnalysisEngine);
		});
		return *(instance.get());
	}


	TradesSummary Analyze(TradesBySymbol tradesBySymbol);
	bool Summarize(std::string symbol, Trades trades, TradeSummaryRec& summary);
};

