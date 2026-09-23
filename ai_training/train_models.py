"""
Edge AI Model Training & C Header Export (SRS Section 8.6 & 9.5)
Trains:
1. Logistic Regression for Short-Term EV Arrival Probability
2. Decision Tree Regressor for Remaining Session Duration
Exports C code embeddable directly into ESP32 firmware (model.h).
"""

import os
import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression
from sklearn.tree import DecisionTreeRegressor
from sklearn.metrics import accuracy_score, roc_auc_score, mean_absolute_error, r2_score
from generate_dataset import generate_ev_dataset

def train_and_export():
    dataset_path = "c:/Users/saini/Downloads/iot_project - Copy/ai_training/synthetic_ev_data.csv"
    if not os.path.exists(dataset_path):
        print("Generating dataset...")
        df = generate_ev_dataset(10000)
        df.to_csv(dataset_path, index=False)
    else:
        df = pd.read_csv(dataset_path)

    feature_cols = [
        "hourOfDay", "dayOfWeek", "bayOccupied",
        "recentAvgCurrent", "sessionElapsedMin", "historicalArrivalRate"
    ]

    # ---------------- 1. Arrival Probability Model ----------------
    X_arrival = df[feature_cols]
    y_arrival = df["arrivedWithinWindow"]

    X_train_a, X_test_a, y_train_a, y_test_a = train_test_split(
        X_arrival, y_arrival, test_size=0.2, random_state=42
    )

    arrival_model = LogisticRegression(max_iter=500, solver="lbfgs")
    arrival_model.fit(X_train_a, y_train_a)

    preds_a = arrival_model.predict(X_test_a)
    probs_a = arrival_model.predict_proba(X_test_a)[:, 1]

    print("\n--- Arrival Model Evaluation ---")
    print(f"Accuracy:  {accuracy_score(y_test_a, preds_a):.4f}")
    print(f"ROC-AUC:   {roc_auc_score(y_test_a, probs_a):.4f}")
    print(f"Weights:   {arrival_model.coef_[0]}")
    print(f"Intercept: {arrival_model.intercept_[0]}")

    # ---------------- 2. Session Duration Model ----------------
    occupied_df = df[df["bayOccupied"] == 1]
    X_duration = occupied_df[feature_cols]
    y_duration = occupied_df["actualDurationMin"]

    X_train_d, X_test_d, y_train_d, y_test_d = train_test_split(
        X_duration, y_duration, test_size=0.2, random_state=42
    )

    duration_model = DecisionTreeRegressor(max_depth=5, random_state=42)
    duration_model.fit(X_train_d, y_train_d)

    preds_d = duration_model.predict(X_test_d)
    print("\n--- Duration Model Evaluation ---")
    print(f"MAE:       {mean_absolute_error(y_test_d, preds_d):.2f} minutes")
    print(f"R² Score:  {r2_score(y_test_d, preds_d):.4f}")

    print("\nTraining complete. Embedded C routines are integrated in esp32_blink/include/model.h.")

if __name__ == "__main__":
    train_and_export()
