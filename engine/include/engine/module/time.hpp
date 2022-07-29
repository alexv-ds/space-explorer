#pragma once
#include <flecs.h>

namespace engine {



struct time {
  time(flecs::world& world);

  struct Timer {
    //in seconds
    float timeout = 0.0f; //через сколько сработает таймер
    float repeat = 0.0f; //если >0, то следующее срабатывание будет через указанное время, иначе компонент удаляется
    float duein = 0.0f; //точка времени, когда сработает таймер
  };

  //вешается на энтитю, когда таймер срабатывает
  //можно свободно удалять
  struct TimerEvent {};

  //Singleton component
  //доступен после импорта модуля
  struct Clock {
    float time; //текущее время кадра
    float delta; //дельта с предыдущего кадра
  };
  
};

using Time = time;


}