#!/bin/bash

# Настройки
PACKAGE_NAME="com.example.mobpos" # package name приложения
ACTIVITY_NAME=".MainActivity"
LOG_FILE="app_logs_$(date +"%Y%m%d_%H%M%S").log"
DURATION=300  # длительность записи логов в секундах (по умолчанию 5 минут)
FILTER_TAG="mobpos" # название тега для фильтрации логов

# Проверяем, подключено ли устройство
DEVICE_CONNECTED=$(adb devices | grep -w "device" | wc -l)

if [ "$DEVICE_CONNECTED" -lt 1 ]; then
  echo "Ошибка: Нет подключенного устройства."
  exit 1
fi

echo "[INFO] Устройство обнаружено. Очищаем предыдущие логи..."
adb logcat -c

echo "[INFO] Запускаем приложение $PACKAGE_NAME/$ACTIVITY_NAME..."
adb shell am start -n "$PACKAGE_NAME/$ACTIVITY_NAME"

echo "[INFO] Начинаем запись логов в файл '$LOG_FILE' на $DURATION секунд..."

# Записываем логи в фоне
adb logcat $FILTER_TAG > "$LOG_FILE" &
LOGCAT_PID=$!

echo "[INFO] Логирование начато. Логи сохраняются в '$LOG_FILE'."

sleep $DURATION

echo "[INFO] Останавливаем логирование и закрываем поток..."
kill $LOGCAT_PID
wait $LOGCAT_PID 2>/dev/null

echo "[INFO] Приложение остановлено."
echo "[INFO] Логи успешно сохранены в файл: $LOG_FILE"