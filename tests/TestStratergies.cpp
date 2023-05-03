#include <gtest/gtest.h>
#include <crypto-platform/Stratergies.h>

TEST(StratergiesTest, FixedTradeSize) {
    Stratergies strat;

    {
        // Generate trade size from function using 1000 trades and units of 10 currency
        double tradeSize = strat.fixedTradeSize(1000, 10);

        // 10/1000 is equal to 0.01
        EXPECT_EQ(tradeSize, 0.01);
    }

    {
        // Generate trade size from function using 1000 trades and units of 10 currency
        double tradeSize = strat.fixedTradeSize(1000, 10);

        // 10/1000 is equal to 0.01 not 0.02. We assert to not equal to
        ASSERT_NE(tradeSize, 0.02);
    }
}

TEST(StratergiesTest, PercentTradeSize) {
    Stratergies strat;

    {
        // Generate trade size form function based on 10%
        double tradeSize = strat.percentTradeSize(10, 10);

        // 10 * 10 / 100 is equal to 1
        EXPECT_EQ(tradeSize, 1);
    }

    {
        // Generate trade size form function based on 50%
        double tradeSize = strat.percentTradeSize(50, 10);

        // 10 * 50 / 100 is equal to 5
        EXPECT_EQ(tradeSize, 5);
    }

    {
        // Generate trade size form function based on 0%
        double tradeSize = strat.percentTradeSize(0, 10);

        // 10 * 0 / 100 is equal to 0
        EXPECT_EQ(tradeSize, 0);
    }

    {
        // Generate trade size form function based on 100%
        double tradeSize = strat.percentTradeSize(100, 10);

        // 10 * 100 / 100 is equal to 10
        EXPECT_EQ(tradeSize, 10);
    }
}

TEST(StratergiesTest, VolatilityTradeSize) {
    Stratergies strat;

    {
        double prices[] = {100.0, 105.0, 110.0, 115.0, 120.0};
        double volatility = strat.volatilityTradeSize(prices, 5);
        double trade_size_multiplier = strat.calculate_trade_size_multiplier(volatility);
        double base_trade_size = 100.0;
        double adjusted_trade_size = strat.adjust_trade_size(base_trade_size, trade_size_multiplier);

        EXPECT_EQ(adjusted_trade_size, 107.071);

    }
    
}