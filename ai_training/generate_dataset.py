"""
Synthetic EV Charging Dataset Generator (SRS Section 8.6.4)
Generates realistic multi-day EV charging station behavior with:
- Commute peak hours (8-10 AM, 6-9 PM)
- Weekday vs weekend variations
- Correlated session arrival State of Charge (SoC) and charging duration
"""

import numpy as np
import pandas as pd

def generate_ev_dataset(num_samples=10000, random_seed=42):
    np.random.seed(random_seed)
    
    records = []
    
    for _ in range(num_samples):
        hour = np.random.randint(0, 24)
        day_of_week = np.random.randint(0, 7) # 0 = Sunday, 6 = Saturday
        is_weekend = (day_of_week == 0 or day_of_week == 6)
        
        # Historical arrival profile
        if 8 <= hour <= 10:
            base_prob = 0.55 if is_weekend else 0.65
        elif 18 <= hour <= 21:
            base_prob = 0.50 if is_weekend else 0.60
        elif 0 <= hour <= 5:
            base_prob = 0.05
        elif 11 <= hour <= 17:
            base_prob = 0.30
        else:
            base_prob = 0.20
            
        hist_arrival_rate = base_prob
        
        # Bay occupancy state
        bay_occupied = 1 if np.random.rand() < (base_prob * 1.2) else 0
        
        if bay_occupied == 1:
            # Session elapsed time (0 to 240 mins)
            session_elapsed = np.random.exponential(scale=45.0)
            session_elapsed = min(session_elapsed, 240.0)
            
            # Initial arrival SoC (20% to 80%)
            arrival_soc = np.random.uniform(20.0, 80.0)
            
            # Charging rate current (10A to 32A)
            recent_avg_current = np.random.normal(loc=16.0, scale=4.0)
            recent_avg_current = np.clip(recent_avg_current, 6.0, 32.0)
            
            # Total charging session duration needed to reach ~95% SoC (approx 30 to 210 mins)
            battery_capacity_kwh = np.random.choice([40, 60, 75])
            energy_needed_kwh = battery_capacity_kwh * (0.95 - (arrival_soc / 100.0))
            power_kw = (230.0 * recent_avg_current) / 1000.0
            actual_total_duration = max(20.0, (energy_needed_kwh / power_kw) * 60.0)
            
            # Remaining duration
            remaining_duration = max(0.0, actual_total_duration - session_elapsed)
        else:
            session_elapsed = 0.0
            recent_avg_current = 0.0
            remaining_duration = 0.0
            
        # Target: EV arrival within the next 30-minute window
        arrival_prob = hist_arrival_rate * (0.8 if bay_occupied else 1.2)
        arrived_within_window = 1 if np.random.rand() < arrival_prob else 0
        
        records.append({
            "hourOfDay": hour,
            "dayOfWeek": day_of_week,
            "bayOccupied": bay_occupied,
            "recentAvgCurrent": round(recent_avg_current, 2),
            "sessionElapsedMin": round(session_elapsed, 1),
            "historicalArrivalRate": round(hist_arrival_rate, 3),
            "arrivedWithinWindow": arrived_within_window,
            "actualDurationMin": round(remaining_duration, 1)
        })
        
    df = pd.DataFrame(records)
    return df

if __name__ == "__main__":
    df = generate_ev_dataset(10000)
    output_file = "c:/Users/saini/Downloads/iot_project - Copy/ai_training/synthetic_ev_data.csv"
    df.to_csv(output_file, index=False)
    print(f"Generated {len(df)} records -> {output_file}")
    print(df.head())
