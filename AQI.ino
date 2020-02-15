double PM_2_5_AQI[][4] = {
  {0, 12.0, 0, 50},
  {12.1, 35.4, 51, 100},
  {35.5, 55.4, 101, 150},
  {55.5, 150.4, 151, 200},
  {150.5, 250.4, 201, 300},
  {250.5, 350.4, 301, 400},
  {350.5, 500.4, 401, 500}
};

int PM_2_5_AQI_SIZE = sizeof(PM_2_5_AQI) / sizeof(PM_2_5_AQI[0]);

int calculateAqi(int raw) {
  for (int i = 0; i < PM_2_5_AQI_SIZE; i++) {
    double *row = PM_2_5_AQI[i];
    if (raw < row[1]) {
      return round((row[3] - row[2]) / (row[1] - row[0]) * (raw - row[0])) + row[2];
    }
  }
  return -1;
}
