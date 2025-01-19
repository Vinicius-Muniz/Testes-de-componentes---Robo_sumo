import matplotlib.pyplot as plt

CONSUMPTION = {
    "TCRT5000 (2x)": 20,
    "HC-SR04 (2x)": 30,
    "Motors (no load)": 400,
    "Motors (full load)": 4000,
    "Driver BTS7960 (2x)": 100,
    "Arduino Uno": 50,
}

BATTERY_CAPACITY = 1500 * 2

BATTERY_VOLTAGE = 11.1

SIMULATION_TIME = 60

NORMAL_CONSUMPTION = (
    CONSUMPTION["TCRT5000 (2x)"] +
    CONSUMPTION["HC-SR04 (2x)"] +
    CONSUMPTION["Motors (no load)"] +
    CONSUMPTION["Driver BTS7960 (2x)"] +
    CONSUMPTION["Arduino Uno"]
)

MAX_CONSUMPTION = (
    CONSUMPTION["TCRT5000 (2x)"] +
    CONSUMPTION["HC-SR04 (2x)"] +
    CONSUMPTION["Motors (full load)"] +
    CONSUMPTION["Driver BTS7960 (2x)"] +
    CONSUMPTION["Arduino Uno"]
)

battery_remaining_normal = BATTERY_CAPACITY
battery_remaining_max = BATTERY_CAPACITY
time_elapsed = 0

time_data = []
battery_data_normal = []
battery_data_max = []

while (battery_remaining_normal > 0 or battery_remaining_max > 0) and time_elapsed < SIMULATION_TIME:
    time_data.append(time_elapsed)
    battery_data_normal.append(max(battery_remaining_normal, 0))
    battery_data_max.append(max(battery_remaining_max, 0))

    battery_remaining_normal -= NORMAL_CONSUMPTION / 60
    battery_remaining_max -= MAX_CONSUMPTION / 60

    time_elapsed += 1

plt.plot(time_data, battery_data_normal, label="Modo Normal (Motores sem carga)", color='orange')
plt.plot(time_data, battery_data_max, label="Modo Potência Máxima (Motores carga total)", linestyle='--', color='red')
plt.xlabel("Tempo (minutos)")
plt.ylabel("Bateria Restante (mAh)")
plt.title("Simulação de Consumo da Bateria do Robô")
plt.legend()
plt.grid(True)
plt.show()

autonomia_normal = time_elapsed - 1 if battery_remaining_normal <= 0 else time_elapsed
autonomia_max = time_elapsed - 1 if battery_remaining_max <= 0 else time_elapsed

print(f"Autonomia no modo normal: {autonomia_normal} minutos")
print(f"Autonomia no modo potência máxima: {autonomia_max} minutos")

if battery_remaining_normal <= 0:
    print("O robô desligaria após aproximadamente:", autonomia_normal, "minutos no modo normal.")
if battery_remaining_max <= 0:
    print("O robô desligaria após aproximadamente:", autonomia_max, "minutos no modo de potência máxima.")
