/**
 * @author  Scaxlibur
 * @version V1.0.0
 * @date    2024-10-10 21:12:26
 * @note    信号发生器核心
 **/

#include "wave_gen.hpp"

int waveindex = 2;                 // 当前波形位置
int wave_index_step = 1;           // 波形索引递增步长
//uint16_t* waveTab = new uint16_t[SAMPLE_PER_CYCLE]; // 最终根据配置生成的波形数据
uint16_t waveTab[SAMPLE_PER_CYCLE];

/**
 * @brief               波形发生器构造函数
 * @param   uMaxValue   峰峰值
 * @param   offSetValue 偏置电压
 * @param   duty        占空比
 * @param   freq        频率
 * @param   wave_type   波形种类
 * @retval              无
 * @note                根据以上参数，构造一个波形发生器对象
 */
WAVE_GEN::WAVE_GEN(double uMaxValue, double offSetValue, int duty, unsigned int freq, WAVE_TYPE_t wave_type)
{
    this->uMaxValue = uMaxValue;
    this->offSetValue = offSetValue;
    this->duty = duty;
    this->freq = freq;
    this->wave_type = wave_type;
}

/**
 * @brief                 波形发生器析构函数
 * @param                 无
 * @retval                无
 * @note                  对象销毁前自动调用
 */
WAVE_GEN::~WAVE_GEN() {
  ESP_ERROR_CHECK(gptimer_stop(waveCounterTimer_handle));
  ESP_ERROR_CHECK(gptimer_disable(waveCounterTimer_handle));
  ESP_ERROR_CHECK(gptimer_del_timer(waveCounterTimer_handle));
}

/**
 * @brief   硬件定时器中断函数
 * @param   timer           Timer handle created by `gptimer_new_timer`
 * @param   *edata          Alarm event data, fed by driver
 * @param   *user_ctx       User data, passed from `gptimer_register_event_callbacks`
 * @retval  无
 * @note    函数类型必须是bool
 */
bool wave_alarm_cb_t(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    if (waveindex >= SAMPLE_PER_CYCLE)
    {
        waveindex = 0;
    }
    AD5626_writeValue(waveTab[waveindex]);
    waveindex += wave_index_step;
    return true;
}

/**
 * @brief       配置硬件定时器和报警
 * @retval      无
 */
/*
bool WAVE_GEN::waveAlramTimer_config()
{
  //ESP_ERROR_CHECK(gptimer_new_timer(&waveCounterTimer_config, &waveCounterTimer_handle));             // 第一个参数是配置的地址，第二个参数是配置时钟的句柄地址
  //ESP_ERROR_CHECK(gptimer_set_alarm_action(waveCounterTimer_handle, &waveCounterTimer_alarm_config)); // 配置报警
  // 设定报警触发的回调函数
  const gptimer_event_callbacks_t cbs = {// 下面将要用到的回调函数(必须是布尔类型)，结构体里面写回调函数的指针
                                         .on_alarm = wave_alarm_cb_t
                                         };
  ESP_ERROR_CHECK(gptimer_register_event_callbacks(waveCounterTimer_handle, &cbs, NULL)); // 第一个参数是配置时钟的句柄地址，第二个是回调函数结构体的地址，第三个是传递给回调函数的值
  return true;
}
*/

/**
 * @brief       初始化波形发生器
 * @retval      无
 * @note        生成默认波形，并配置好定时器
 */
void WAVE_GEN::initTimer()
{
    /** 默认输出正弦波 */
    waveGen(wave_type);
    adjust_step();
    /** 设置闹钟每秒调用wave_alarm_cb_t函数1 tick为0.25us   => 1秒为1000000us */
    uint32_t T = (TIME_CLOCK_HZ * wave_index_step) / (freq * SAMPLE_PER_CYCLE);
    this->waveCounterTimer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,     // 时钟源,APB时钟最快不超过80MHz
        .direction = GPTIMER_COUNT_UP,          // 时钟计数方向
        .resolution_hz = TIME_CLOCK_HZ,         // 40MHz, 1 tick = 0.25us             //时钟频率配置
        .intr_priority = 0,                     // 中断优先级
    };
    this->waveCounterTimer_alarm_config = {
        .alarm_count = T,
        .reload_count = 0,
        .flags = {
            .auto_reload_on_alarm = true,
        }};
    /** 启动定时器 */
    ESP_ERROR_CHECK(gptimer_new_timer(&waveCounterTimer_config, &waveCounterTimer_handle));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(waveCounterTimer_handle, &waveCounterTimer_alarm_config));

    const gptimer_event_callbacks_t cbs = {// 下面将要用到的回调函数(必须是布尔类型)，结构体里面写回调函数的指针
        .on_alarm = wave_alarm_cb_t
        };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(waveCounterTimer_handle, &cbs, NULL)); // 第一个参数是配置时钟的句柄地址，第二个是回调函数结构体的地址，第三个是传递给回调函数的值
    
    ESP_ERROR_CHECK(gptimer_enable(waveCounterTimer_handle));
    ESP_ERROR_CHECK(gptimer_start(waveCounterTimer_handle));
}

/**
 * @brief           更新定时器
 * @retval          无
 * @note            在波形发生器频率发生改变后，重设定时器
 */
void WAVE_GEN::updateTimer()
{
    ESP_ERROR_CHECK(gptimer_stop(waveCounterTimer_handle));
    ESP_ERROR_CHECK(gptimer_disable(waveCounterTimer_handle)); // 先关闭定时器
    this->waveCounterTimer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT, // 时钟源
        .direction = GPTIMER_COUNT_UP,      // 时钟计数方向
        .resolution_hz = TIME_CLOCK_HZ,     // 时钟频率配置
        .intr_priority = 0,                 // 中断优先级
    };
    adjust_step();
    uint32_t dacTime = (1000000 * wave_index_step) / (freq * SAMPLE_PER_CYCLE);
    this->waveCounterTimer_alarm_config = {
        .alarm_count = dacTime, // 到达这个数时警报
        .reload_count = 0,      // 重载的数值
        .flags = {
            .auto_reload_on_alarm = true, // 是否自动重载
        }};
    ESP_ERROR_CHECK(gptimer_set_alarm_action(waveCounterTimer_handle, &waveCounterTimer_alarm_config)); // 配置报警
    /** 启动警报 */
    ESP_ERROR_CHECK(gptimer_enable(waveCounterTimer_handle));
    ESP_ERROR_CHECK(gptimer_start(waveCounterTimer_handle));
}

/**
 * @brief             波形表生成函数
 * @param wave_type   波形种类，有SIN、SQUARE、SAWTOOTH
 * @retval            无
 * @note              根据波形发生器的参数生成相应的波形，并将其储存到waveTab[]数组中
 */
void WAVE_GEN::waveGen(WAVE_TYPE_t wave_type)
{
    double sineValue = 0.0;
    float x = SAMPLE_PER_CYCLE * ((float)duty / 100.0);
    switch(wave_type)
    {
        default:
        case SIN:   // 正弦波
            for (int i = 0; i < SAMPLE_PER_CYCLE; i++)
            {
            // sineValue = sin(((2 * PI) / SAMPLE_PER_CYCLE) * i) * (uMaxValue / 2) + offSetValue;
            sineValue = sin(((2 * PI) / SAMPLE_PER_CYCLE) * i) + 1;

            // waveTab1[i] = (((int)(sineValue * ADC_MAX_VALUE / ADC_MAX_VOLTAGE)));
            waveTab1[i] = ((uint16_t)(sineValue * (ADC_MAX_VALUE / 2)));
            }
            break;
        case SQUARE:
            for (int i = 0; i < SAMPLE_PER_CYCLE; i++)
            {
            if (i < x)
            {
                // waveTab1[i] = (int)(ADC_MAX_VALUE * (uMaxValue / 2 + offSetValue) / ADC_MAX_VOLTAGE);
                waveTab1[i] = (uint8_t)(ADC_MAX_VALUE * (uMaxValue / 2 + offSetValue));
            }
            else
            {
                // waveTab1[i] = (uint8_t)(ADC_MAX_VALUE * (-(uMaxValue / 2) + offSetValue) / ADC_MAX_VOLTAGE);
                waveTab1[i] = (uint8_t)(ADC_MAX_VALUE * (-(uMaxValue / 2) + offSetValue));
            }
            }
            break;
        case SAWTOOTH:
            for (int i = -(SAMPLE_PER_CYCLE / 2); i < (SAMPLE_PER_CYCLE / 2); i++)
            {
                // waveTab1[i + (SAMPLE_PER_CYCLE / 2)] = (uint8_t)(i * (uMaxValue / ADC_MAX_VOLTAGE) + (offSetValue * ADC_MAX_VALUE / ADC_MAX_VOLTAGE));
                waveTab1[i + (SAMPLE_PER_CYCLE / 2)] = (uint8_t)(i * (uMaxValue / ADC_MAX_VOLTAGE));
            }
    }
    for (int i = 0; i < SAMPLE_PER_CYCLE; i++)
    {
        if (waveTab1[i] > ADC_MAX_VALUE)
        {
        waveTab1[i] = ADC_MAX_VALUE;
        }
        if (waveTab1[i] < 0)
        {
        waveTab1[i] = 0;
        }
        waveTab[i] = waveTab1[i];
    }
    
    printf("waveTab1[123] = %u\n", waveTab1[123]);
    printf("waveTab1[512] = %u\n", waveTab1[512]);
    printf("waveTab[123] = %u\n", waveTab[123]);
    printf("waveTab[512] = %u\n", waveTab[512]);
}

/**
 * @brief       根据频率自动切换跳跃步长
 * @retval      无
 * @note        0-100hz时256个数全部依次输出，100-200hz隔一个输出一个，200-300隔2个输出一个···
 */
void WAVE_GEN::adjust_step()
{
    //if (freq > 0 && freq <= 100)
    //{
        wave_index_step = 5;
    //}
    //else if (freq > 100 && freq <= 1500)
    //{
    //    wave_index_step = (int)(freq / 100) + 1;
    //}
    //else
    //{
    //}
}

/**
 * @brief         波形切换函数
 * @param wave    波形种类
 * @retval        无
 * @note          无
 */
void WAVE_GEN::waveSelect(int wave)
{
    if (wave < 1 || wave > 3)
    {
        // Serial.println("波形种类设置错误");
        return;
    }
    wave_type = (enum WAVE_TYPE_t)(wave);
    waveGen(wave_type);
}

/**
 * @brief 设置波形发生器峰峰值
 * @param value 峰峰值大小
 * @retval 0:设置成功 -1:超出范围
 * @note 无
 */
int WAVE_GEN::set_uMaxValue(double value)
{
    if (value >= 0 && value <= 3.3)
    {
        uMaxValue = value;
        waveGen(wave_type);
        return 0;
    }
    else
    {
        // Serial.println("峰峰值设置超出范围0-3.3V");
        return -1;
    }
}

/**
 * @brief 设置波形发生器偏置电压
 * @param value:偏置电压大小
 * @retval 0:设置成功 -1:超出范围
 * @note 无
 */
int WAVE_GEN::set_offSetValue(double value)
{
    if (value >= 0 && value <= 3.3)
    {
        offSetValue = value;
        waveGen(wave_type);
        return 0;
    }
    else
    {
        // Serial.println("偏置电压设置超出范围0-3.3V");
        return -1;
    }
}

/**
 * @brief 设置波形发生器方波占空比
 * @param value:占空比大小
 * @retval 0:设置成功 -1:超出范围
 * @note 无
 */
int WAVE_GEN::set_duty(int value)
{
    if (value >= 0 && value <= 100)
    {
        duty = value;
        waveGen(wave_type);
        return 0;
    }
    else
    {
        // Serial.println("占空比设置超出范围0-100");
        return -1;
    }
}

/**
 * @brief 设置波形发生器信号频率
 * @param value:频率值
 * @retval 0:设置成功 -1:超出范围
 * @note 无
 */
int WAVE_GEN::set_freq(int value)
{
    if (value > 0 && value <= 1500)
    {
        freq_old = freq;
        freq = value;
        if (freq != freq_old)
        updateTimer();
        return 0;
    }
    else
    {
        // Serial.println("频率设置超出范围0~1.5kHz");
        return -1;
    }
}