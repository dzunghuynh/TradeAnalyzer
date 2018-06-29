#include "stdafx.h"
#include "AnalysisEngine.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

std::unique_ptr<AnalysisEngine> AnalysisEngine::instance;
std::once_flag AnalysisEngine::onceFlag;

/*
Find the following on a per symbol basis:
- Maximum time gap
(time gap = Amount of time that passes between consecutive trades of a symbol)
if only 1 trade is in the file then the gap is 0.
- Total Volume traded (Sum of the quantity for all trades in a symbol).
- Max Trade Price.
- Weighted Average Price.  Average price per unit traded not per trade.
Result should be truncated to whole numbers.

Example: the following trades
20 shares of aaa @ 18
5 shares of aaa @ 7
Weighted Average Price = ((20 * 18) + (5 * 7)) / (20 + 5) = 15

*/

TradesSummary AnalysisEngine::Analyze(TradesBySymbol tradesBySymbol)
{
	TradesSummary tradeSummary;

	if (tradesBySymbol.empty())
		return tradeSummary;

	for (auto&&[k, v] : tradesBySymbol)
	{
		//std::cout << k << std::endl;
		std::sort(v.begin(), v.end(),
			[](const std::shared_ptr<TradeRec>& lhs, const std::shared_ptr<TradeRec>& rhs)
		{ return lhs->Timestamp < rhs->Timestamp; });

		std::shared_ptr<TradeSummaryRec> summaryPtr(new TradeSummaryRec);
		if (!Summarize(k, v, *summaryPtr))
			continue;

		tradeSummary.push_back(summaryPtr);
	}

	return tradeSummary;
}

bool AnalysisEngine::Summarize(std::string symbol, Trades trades, TradeSummaryRec& summary)
{
	if (trades.empty())
		return false;

	int totalCost = 0;

	summary.Symbol = symbol;

	std::shared_ptr<Trade> p = *(trades.begin());
	summary.MaxPrice = p->Price;
	summary.Volume = p->Quantity;
	summary.MaxTimeGap = 0;
	totalCost = p->TransactionAmount();

	if (trades.size() > 1)
	{
		for (auto it = trades.begin() + 1; it != trades.end(); it++)
		{
			// accumulative qty
			summary.Volume += (*it)->Quantity;

			// max price
			if (summary.MaxPrice < (*it)->Price)
				summary.MaxPrice = (*it)->Price;

			// total cost of transac
			totalCost += (*it)->TransactionAmount();

			// calc. gap
			p = *(it - 1);
			auto gap = (*it)->Timestamp - p->Timestamp;
			if (gap > summary.MaxTimeGap)
				summary.MaxTimeGap = (long)gap;
		}
	}

	if (summary.Volume > 0)
		summary.WeightedAveragePrice = (int)((double)totalCost / (double)summary.Volume);

	return true;
}

