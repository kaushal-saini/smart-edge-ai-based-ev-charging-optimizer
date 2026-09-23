/**
 * Smart EV Charging Station Optimizer - Interactive Presentation & Simulator Engine
 */

document.addEventListener("DOMContentLoaded", () => {
  // -------------------------------------------------------------------
  // Presentation Slide State & Narrations
  // -------------------------------------------------------------------
  const slides = document.querySelectorAll(".slide");
  const totalSlides = slides.length;
  let currentSlideIndex = 0;
  let isAutoPlaying = false;
  let autoPlayTimer = null;
  let isMuted = false;

  const narrations = [
    "Welcome to the technical internship presentation on the Smart EV Charging Station Optimizer. In this project, we designed and simulated an intelligent multi-bay charging node using ESP32, Edge AI, MQTT, and ThingsBoard IoT cloud.",
    "The core problem we address is grid congestion and peak-demand power spikes. When multiple electric vehicles plug in at once, unmanaged charging risks tripping grid transformers, inflates operating costs during peak tariff windows, and provides no predictive visibility to operators.",
    "Our system architecture features three layers: The Edge Layer with ESP32 microcontrollers performing sensing and lightweight AI inference, the Communication Layer using MQTT v3.1.1 JSON messaging, and the Cloud Layer on ThingsBoard for live monitoring, alerts, and remote RPC control.",
    "Here is the hardware configuration simulated in Wokwi. We connect calibrated potentiometers for voltage on GPIO 34 and current on GPIO 35, a DHT22 on GPIO 15, plug-in and plug-out push buttons on GPIO 32 and 33, status LEDs on GPIO 18, 19, and 21, and a power relay on GPIO 26.",
    "For the Edge AI engine, we developed two lightweight models: A Logistic Regression model predicting short-term vehicle arrival probability, and a Decision Tree Regressor predicting remaining session duration. Both run natively on the ESP32 in under 5 milliseconds with zero cloud latency.",
    "The local optimization algorithm runs every 5 seconds on each ESP32. It checks for overcurrent safety limits, station power caps, and peak-tariff deferral windows, applying dynamic duty-cycle PWM relay modulation to throttle power without cutting off charging.",
    "For IoT cloud integration, the firmware streams JSON telemetry to ThingsBoard, synchronizes shared attributes for dynamic threshold adjustments, and handles bidirectional RPC commands like setRelayState and setThrottle.",
    "Through this internship, we mastered embedded C++ modular design, TinyML on microcontrollers, resilient MQTT communication, and real-time power grid optimization.",
    "Now let's demonstrate the live working simulator. You can press the Plug-In button, adjust voltage, current, and temperature, observe overcurrent throttling, watch the serial monitor, and trigger remote ThingsBoard RPC overrides."
  ];

  const prevBtn = document.getElementById("prevBtn");
  const nextBtn = document.getElementById("nextBtn");
  const autoPlayBtn = document.getElementById("autoPlayBtn");
  const currentSlideNum = document.getElementById("currentSlideNum");
  const totalSlidesNum = document.getElementById("totalSlidesNum");
  const narratorText = document.getElementById("narratorText");
  const muteNarrationBtn = document.getElementById("muteNarrationBtn");

  totalSlidesNum.textContent = totalSlides;

  // Speech Synthesis & Auto-Play Controller
  let currentUtterance = null;
  let slideAdvanceTimeout = null;

  function showSlide(index, autoAdvanceAfterSpeech = false) {
    if (index < 0) index = 0;
    if (index >= totalSlides) index = totalSlides - 1;
    currentSlideIndex = index;

    slides.forEach((slide, i) => {
      slide.classList.toggle("active", i === currentSlideIndex);
    });

    currentSlideNum.textContent = currentSlideIndex + 1;
    prevBtn.disabled = currentSlideIndex === 0;
    nextBtn.disabled = currentSlideIndex === totalSlides - 1;

    // Update Narration & Speech
    const text = narrations[currentSlideIndex] || "";
    narratorText.textContent = text;
    
    speakNarration(text, () => {
      // Called when speech finishes
      if (isAutoPlaying && autoAdvanceAfterSpeech) {
        if (currentSlideIndex < totalSlides - 1) {
          // Pause 1.5 seconds after audio finishes before advancing to next slide
          slideAdvanceTimeout = setTimeout(() => {
            if (isAutoPlaying) {
              showSlide(currentSlideIndex + 1, true);
            }
          }, 1500);
        } else {
          // Reached end of presentation
          stopAutoPlay();
        }
      }
    });
  }

  function speakNarration(text, onComplete) {
    clearTimeout(slideAdvanceTimeout);
    
    if ("speechSynthesis" in window) {
      window.speechSynthesis.cancel(); // Stop previous audio
    }

    if (isMuted || !("speechSynthesis" in window)) {
      // Fallback timer based on reading speed (~3.5 words per sec)
      if (onComplete) {
        const words = text.split(" ").length;
        const fallbackDurationMs = Math.max(4000, (words / 3.2) * 1000);
        slideAdvanceTimeout = setTimeout(() => {
          onComplete();
        }, fallbackDurationMs);
      }
      return;
    }

    const utterance = new SpeechSynthesisUtterance(text);
    utterance.rate = 1.0;
    utterance.pitch = 1.0;
    currentUtterance = utterance;

    utterance.onend = () => {
      if (onComplete) onComplete();
    };

    utterance.onerror = (e) => {
      console.warn("Speech error or cancelled:", e);
      if (onComplete) onComplete();
    };

    window.speechSynthesis.speak(utterance);
  }

  prevBtn.addEventListener("click", () => {
    stopAutoPlay();
    showSlide(currentSlideIndex - 1, false);
  });

  nextBtn.addEventListener("click", () => {
    stopAutoPlay();
    showSlide(currentSlideIndex + 1, false);
  });

  document.addEventListener("keydown", (e) => {
    if (e.key === "ArrowLeft") {
      stopAutoPlay();
      showSlide(currentSlideIndex - 1, false);
    } else if (e.key === "ArrowRight") {
      stopAutoPlay();
      showSlide(currentSlideIndex + 1, false);
    } else if (e.key === " ") {
      // Spacebar toggles play/pause
      e.preventDefault();
      toggleAutoPlay();
    }
  });

  muteNarrationBtn.addEventListener("click", () => {
    isMuted = !isMuted;
    muteNarrationBtn.textContent = isMuted ? "🔇 Sound OFF" : "🔊 Sound ON";
    if (isMuted && "speechSynthesis" in window) {
      window.speechSynthesis.cancel();
    } else if (!isMuted) {
      speakNarration(narrations[currentSlideIndex], null);
    }
  });

  function startAutoPlay() {
    isAutoPlaying = true;
    autoPlayBtn.textContent = "⏸️ Pause Presentation";
    autoPlayBtn.classList.replace("btn-primary", "btn-secondary");
    showSlide(currentSlideIndex, true);
  }

  function stopAutoPlay() {
    isAutoPlaying = false;
    clearTimeout(slideAdvanceTimeout);
    if ("speechSynthesis" in window) {
      window.speechSynthesis.cancel();
    }
    autoPlayBtn.textContent = currentSlideIndex >= totalSlides - 1 
      ? "🎙️ Restart Oral Presentation" 
      : "▶️ Resume Oral Presentation";
    autoPlayBtn.classList.replace("btn-secondary", "btn-primary");
  }

  function toggleAutoPlay() {
    if (isAutoPlaying) {
      stopAutoPlay();
    } else {
      if (currentSlideIndex >= totalSlides - 1) {
        currentSlideIndex = 0;
      }
      startAutoPlay();
    }
  }

  autoPlayBtn.addEventListener("click", () => {
    toggleAutoPlay();
  });

  // Initialize first slide on load (audio does not auto-advance until user clicks start)
  showSlide(0, false);

  // -------------------------------------------------------------------
  // Interactive Simulator Engine (ESP32 + Edge AI + Optimization + TB)
  // -------------------------------------------------------------------
  let bayStatus = "FREE";
  let voltage = 230.0;
  let current = 0.0;
  let power = 0.0;
  let energyWh = 0.0;
  let temperature = 27.0;
  let predictedArrivalProb = 0.58;
  let predictedDurationMin = 0;
  let loadDecision = "ALLOW";
  let throttleLevel = 100;
  let overloadActive = false;
  let manualOverrideActive = false;
  let sessionStartMs = 0;
  let overloadCurrentA = 16.0;
  let maxStationLoadW = 6000.0;

  // DOM Elements for simulator
  const simBtnPlugin = document.getElementById("simBtnPlugin");
  const simBtnPlugout = document.getElementById("simBtnPlugout");
  const simBayStateBadge = document.getElementById("simBayStateBadge");
  const simVoltSlider = document.getElementById("simVoltSlider");
  const simCurrSlider = document.getElementById("simCurrSlider");
  const simTempSlider = document.getElementById("simTempSlider");
  const voltValDisplay = document.getElementById("voltValDisplay");
  const currValDisplay = document.getElementById("currValDisplay");
  const tempValDisplay = document.getElementById("tempValDisplay");

  const simLedGreen = document.getElementById("simLedGreen");
  const simLedYellow = document.getElementById("simLedYellow");
  const simLedRed = document.getElementById("simLedRed");
  const simRelayVisual = document.getElementById("simRelayVisual");

  const tbPowerVal = document.getElementById("tbPowerVal");
  const tbEnergyVal = document.getElementById("tbEnergyVal");
  const tbArrProbVal = document.getElementById("tbArrProbVal");
  const tbDurVal = document.getElementById("tbDurVal");
  const tbDecisionPill = document.getElementById("tbDecisionPill");
  const tbThrottleVal = document.getElementById("tbThrottleVal");

  const rpcForceOnBtn = document.getElementById("rpcForceOnBtn");
  const rpcForceOffBtn = document.getElementById("rpcForceOffBtn");
  const rpcThrottle50Btn = document.getElementById("rpcThrottle50Btn");
  const rpcClearOverrideBtn = document.getElementById("rpcClearOverrideBtn");
  const rpcStatusText = document.getElementById("rpcStatusText");
  const serialOutput = document.getElementById("serialOutput");

  function logSerial(msg) {
    if (!serialOutput) return;
    const time = new Date().toISOString().substring(11, 19);
    serialOutput.innerHTML += `[${time}] ${msg}\n`;
    serialOutput.scrollTop = serialOutput.scrollHeight;
  }

  // Initial Serial Welcome Banner
  logSerial("=======================================================");
  logSerial("  Smart EV Charging Station Optimizer (Edge AI)");
  logSerial("  ESP32 + MQTT + ThingsBoard + Lightweight ML");
  logSerial("=======================================================");
  logSerial("[WiFi] Connected to Wokwi-GUEST. IP: 10.0.1.42");
  logSerial("[MQTT] Connected to mqtt.thingsboard.cloud:1883");
  logSerial("[SHARED ATTRS] maxStationLoadW=6000 overloadCurrentA=16.0");

  function updateSensorsAndState() {
    voltage = parseFloat(simVoltSlider.value);
    voltValDisplay.textContent = `${voltage.toFixed(1)} V`;

    temperature = parseFloat(simTempSlider.value);
    tempValDisplay.textContent = `${temperature.toFixed(1)} °C`;

    if (bayStatus === "CHARGING") {
      current = parseFloat(simCurrSlider.value);
    } else {
      current = 0.0;
    }
    currValDisplay.textContent = `${current.toFixed(1)} A`;

    power = voltage * current;

    // Energy accumulation
    if (bayStatus === "CHARGING") {
      energyWh += power * (1.0 / 3600.0);
    }

    // Edge AI Inference
    const hourOfDay = new Date().getHours();
    const histRate = (hourOfDay >= 8 && hourOfDay <= 10) || (hourOfDay >= 18 && hourOfDay <= 21) ? 0.62 : 0.25;
    const rawArrivalScore = -2.57 + (hourOfDay * 0.0145) + (histRate * 4.8);
    predictedArrivalProb = 1.0 / (1.0 + Math.exp(-rawArrivalScore));

    if (bayStatus === "CHARGING") {
      const elapsedMin = sessionStartMs > 0 ? (Date.now() - sessionStartMs) / 60000.0 : 0;
      predictedDurationMin = Math.max(5, Math.round(180.0 - elapsedMin * 1.2 - current * 2.5));
    } else {
      predictedDurationMin = 0;
    }

    // Local Optimization
    if (!manualOverrideActive) {
      overloadActive = false;
      if (bayStatus !== "CHARGING") {
        loadDecision = "ALLOW";
        throttleLevel = 100;
      } else {
        if (current > overloadCurrentA) {
          loadDecision = "THROTTLE";
          throttleLevel = 50;
          overloadActive = true;
        } else if (power > maxStationLoadW) {
          loadDecision = "THROTTLE";
          throttleLevel = 70;
        } else {
          loadDecision = "ALLOW";
          throttleLevel = 100;
        }
      }
    }

    // Update Visuals
    updateUI();
  }

  function updateUI() {
    // Bay badge
    simBayStateBadge.textContent = `STATE: ${bayStatus}`;
    simBayStateBadge.className = `bay-badge state-${bayStatus.toLowerCase()}`;

    // LEDs
    simLedGreen.classList.toggle("on", bayStatus === "FREE" && !overloadActive);
    simLedYellow.classList.toggle("on", bayStatus === "CHARGING" && !overloadActive);
    simLedRed.classList.toggle("on", overloadActive);

    // Relay switch
    const relayActive = bayStatus === "CHARGING" && throttleLevel > 0;
    simRelayVisual.classList.toggle("active", relayActive);

    // ThingsBoard Dashboard Widgets
    tbPowerVal.innerHTML = `${power.toFixed(1)} <small>W</small>`;
    tbEnergyVal.textContent = `Energy: ${energyWh.toFixed(2)} Wh`;
    tbArrProbVal.textContent = predictedArrivalProb.toFixed(2);
    tbDurVal.innerHTML = `${predictedDurationMin} <small>min</small>`;

    tbDecisionPill.textContent = loadDecision;
    tbDecisionPill.className = `tb-decision-pill pill-${loadDecision.toLowerCase().includes("throttle") ? "throttle" : "allow"}`;
    tbThrottleVal.textContent = `Throttle Level: ${throttleLevel}%`;
  }

  // Button Listeners
  simBtnPlugin.addEventListener("click", () => {
    if (bayStatus !== "CHARGING") {
      bayStatus = "CHARGING";
      sessionStartMs = Date.now();
      energyWh = 0.0;
      manualOverrideActive = false;
      logSerial("[EVENT] Plug-In Button Pressed (GPIO 32) -> Bay Status: CHARGING");
      updateSensorsAndState();
    }
  });

  simBtnPlugout.addEventListener("click", () => {
    if (bayStatus === "CHARGING") {
      bayStatus = "FREE";
      sessionStartMs = 0;
      manualOverrideActive = false;
      throttleLevel = 0;
      logSerial("[EVENT] Plug-Out Button Pressed (GPIO 33) -> Bay Status: FREE");
      updateSensorsAndState();
    }
  });

  simVoltSlider.addEventListener("input", updateSensorsAndState);
  simCurrSlider.addEventListener("input", updateSensorsAndState);
  simTempSlider.addEventListener("input", updateSensorsAndState);

  // RPC Controls
  rpcForceOnBtn.addEventListener("click", () => {
    manualOverrideActive = true;
    throttleLevel = 100;
    loadDecision = "ALLOW";
    rpcStatusText.textContent = "Mode: RPC Manual Override (Force ON)";
    logSerial("[MQTT <<] v1/devices/me/rpc/request/1 -> {\"method\":\"setRelayState\",\"params\":{\"state\":true}}");
    logSerial("[MQTT >>] v1/devices/me/rpc/response/1 -> {\"success\":true,\"throttleLevel\":100}");
    updateSensorsAndState();
  });

  rpcForceOffBtn.addEventListener("click", () => {
    manualOverrideActive = true;
    throttleLevel = 0;
    loadDecision = "MANUAL_OFF";
    rpcStatusText.textContent = "Mode: RPC Manual Override (Force OFF)";
    logSerial("[MQTT <<] v1/devices/me/rpc/request/2 -> {\"method\":\"setRelayState\",\"params\":{\"state\":false}}");
    logSerial("[MQTT >>] v1/devices/me/rpc/response/2 -> {\"success\":true,\"throttleLevel\":0}");
    updateSensorsAndState();
  });

  rpcThrottle50Btn.addEventListener("click", () => {
    manualOverrideActive = true;
    throttleLevel = 50;
    loadDecision = "MANUAL_THROTTLE";
    rpcStatusText.textContent = "Mode: RPC Manual Override (Throttle 50%)";
    logSerial("[MQTT <<] v1/devices/me/rpc/request/3 -> {\"method\":\"setThrottle\",\"params\":{\"level\":50}}");
    logSerial("[MQTT >>] v1/devices/me/rpc/response/3 -> {\"success\":true,\"throttleLevel\":50}");
    updateSensorsAndState();
  });

  rpcClearOverrideBtn.addEventListener("click", () => {
    manualOverrideActive = false;
    rpcStatusText.textContent = "Mode: Autonomous Optimization";
    logSerial("[MQTT <<] v1/devices/me/rpc/request/4 -> {\"method\":\"clearOverride\"}");
    logSerial("[MQTT >>] v1/devices/me/rpc/response/4 -> {\"success\":true,\"manualOverrideActive\":false}");
    updateSensorsAndState();
  });

  // Main 1-second simulation clock & periodic MQTT telemetry cycle
  setInterval(() => {
    updateSensorsAndState();
  }, 1000);

  setInterval(() => {
    const telemetryJson = JSON.stringify({
      bayId: "BAY1",
      voltage: parseFloat(voltage.toFixed(1)),
      current: parseFloat(current.toFixed(1)),
      power: parseFloat(power.toFixed(1)),
      energyWh: parseFloat(energyWh.toFixed(2)),
      temperature: parseFloat(temperature.toFixed(1)),
      bayStatus: bayStatus,
      predictedArrivalProb: parseFloat(predictedArrivalProb.toFixed(3)),
      predictedDurationMin: predictedDurationMin,
      loadDecision: loadDecision,
      throttleLevel: throttleLevel,
      overloadActive: overloadActive,
      manualOverrideActive: manualOverrideActive
    });
    logSerial(`[MQTT >> Telemetry] ${telemetryJson}`);
  }, 5000);

  updateSensorsAndState();
});
