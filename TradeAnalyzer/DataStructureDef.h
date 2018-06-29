#pragma once
#include <string>
#include <vector>
#include <map>

typedef struct Trade
{
	// <TimeStamp>,<Symbol>,<Quantity>,<Price>

	uint64_t	Timestamp;
	std::string Symbol;
	int			Quantity;
	int			Price;

	Trade()
		: Timestamp{ 0 }, Symbol{ "" }, Quantity{ 0 }, Price{ 0 }
	{}
	Trade(uint64_t timestamp, std::string symbol, int quantity, int price)
		: Timestamp{ timestamp }, Symbol{ symbol }, Quantity{ quantity }, Price{ price }
	{}

	int			TransactionAmount() { return (Price * Quantity); }

} TradeRec, *TradeRecPtr;

typedef std::vector<std::shared_ptr<TradeRec>>	Trades;
typedef std::map<std::string, Trades>			TradesBySymbol;

typedef struct TradeSummary
{
	// <symbol>,<MaxTimeGap>,<Volume>,<WeightedAveragePrice>,<MaxPrice>

	std::string Symbol;
	long		MaxTimeGap;
	int			Volume;
	int			WeightedAveragePrice;
	int			MaxPrice;

	TradeSummary()
		: Symbol{ "" }, MaxTimeGap{ 0 }, Volume{ 0 }, WeightedAveragePrice{ 0 }, MaxPrice{ 0 }
	{}
	TradeSummary(std::string symbol, long timeGap, int volume, int weightedAveragePrice, int maxPrice)
		: Symbol{ symbol }, MaxTimeGap{ timeGap }, Volume{ volume }, WeightedAveragePrice{ weightedAveragePrice }, MaxPrice{ maxPrice }
	{}
} TradeSummaryRec, *TradeSummaryRecPtr;

typedef std::vector<std::shared_ptr<TradeSummaryRec>>	TradesSummary;

