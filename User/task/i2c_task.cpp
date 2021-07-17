/// @file      task/i2c_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "i2c_task.h"
#include "device/at42qt1070.h"
#include "device/gyro.h"
#include "device/level_meter.h"
#include "device/pca9635.h"
#include "device/rotary_encoder.h"
#include "device/ssd1306.h"
#include "message/msglib.h"
#include "stm32f7xx_ll_dma.h"
#include "task/app_task.h"
#include "task/i2c_monitor_task.h"
#include "task/sound_task.h"

namespace
{
satoh::I2C *s_i2c = 0; ///< I2C通信オブジェクト

/// @brief ジャイロ値取得処理
/// @param[in] mpu6050 MPU6050と通信するオブジェクト
/// @param[in] icm20602 ICM20602と通信するオブジェクト
void gyroGetProc(satoh::Gyro const &mpu6050, satoh::Gyro const &icm20602) noexcept
{
  if (mpu6050.ok())
  {
    satoh::msg::ACC_GYRO ag{};
    if (mpu6050.getAccelGyro(ag.acc, ag.gyro))
    {
      satoh::sendMsg(appTaskHandle, satoh::msg::GYRO_NOTIFY, &ag, sizeof(ag));
    }
    return;
  }
  if (icm20602.ok())
  {
    satoh::msg::ACC_GYRO ag{};
    if (icm20602.getAccelGyro(ag.acc, ag.gyro))
    {
      satoh::sendMsg(appTaskHandle, satoh::msg::GYRO_NOTIFY, &ag, sizeof(ag));
    }
    return;
  }
}
/// @brief レベルメーター更新処理
/// @param[in] level レベルメーターと通信するオブジェクト
/// @param[in] msg リクエストメッセージ
void ledLevelUpdateProc(satoh::LevelMeter &level, satoh::Message const *msg) noexcept
{
  if (level.ok())
  {
    auto *param = reinterpret_cast<satoh::msg::LED_LEVEL const *>(msg->bytes);
    level.setLeft(param->left);
    level.setRight(param->right);
    level.show();
  }
}
/// @brief Power/Modulation LED点灯・消灯処理
/// @param[in] level レベルメーターと通信するオブジェクト
/// @param[in] msg リクエストメッセージ
void ledSimpleProc(satoh::LevelMeter &level, satoh::Message const *msg) noexcept
{
  if (level.ok())
  {
    auto *param = reinterpret_cast<satoh::msg::LED_SIMPLE const *>(msg->bytes);
    if (param->led == 0)
    {
      level.setPower(param->level);
    }
    else
    {
      level.setModulation(param->level);
    }
    level.show();
  }
}
/// @brief エフェクトLED更新処理（LED指定）
/// @param[in] led エフェクトLEDと通信するオブジェクト
/// @param[in] msg リクエストメッセージ
void ledEffectProc(satoh::PCA9635 &led, satoh::Message const *msg) noexcept
{
  if (led.ok())
  {
    auto *param = reinterpret_cast<satoh::msg::LED_EFFECT const *>(msg->bytes);
    led.set(param->rgb, param->led);
  }
}
/// @brief エフェクトLED更新処理（全LED）
/// @param[in] led エフェクトLEDと通信するオブジェクト
/// @param[in] msg リクエストメッセージ
void ledAllEffectProc(satoh::PCA9635 &led, satoh::Message const *msg) noexcept
{
  if (led.ok())
  {
    auto *param = reinterpret_cast<satoh::msg::LED_ALL_EFFECT const *>(msg->bytes);
    led.set(param->rgb);
  }
}
/// @brief キー状態取得処理
void keyUpdateProc(satoh::AT42QT1070 &modeKey)
{
  if (modeKey.ok())
  {
    uint8_t keys[6] = {};
    if (modeKey.read(keys) <= 0)
    {
      return;
    }
    satoh::msg::MODE_KEY msg{};
    msg.tap = keys[0];
    msg.re1 = keys[1];
    msg.down = keys[2];
    msg.ok = keys[3];
    msg.up = keys[4];
    msg.rtn = keys[5];
    satoh::sendMsg(appTaskHandle, satoh::msg::MODE_KEY_NOTIFY, &msg, sizeof(msg));
  }
}
/// @brief エンコーダ状態取得処理
void encoderGetProc(satoh::RotaryEncoder &encoder)
{
  if (encoder.ok())
  {
    satoh::msg::ROTARY_ENCODER enc{};
    satoh::msg::EFFECT_KEY key{};
    int res = encoder.read(key.button, enc.angleDiff);
    if (res & 1)
    {
      satoh::sendMsg(appTaskHandle, satoh::msg::ROTARY_ENCODER_NOTIFY, &enc, sizeof(enc));
    }
    if (res & 2)
    {
      satoh::sendMsg(appTaskHandle, satoh::msg::EFFECT_KEY_CHANGED_NOTIFY, &key, sizeof(key));
    }
  }
}
/// @brief OLED - エフェクターパラメータ一一覧表示
/// @param[in] oled OLED通信オブジェクト
/// @param[in] msg リクエストメッセージ
void oledDispEffector(satoh::SSD1306 &oled, satoh::Message const *msg)
{
  if (oled.ok())
  {
    auto *param = reinterpret_cast<satoh::msg::OLED_DISP_EFFECTOR const *>(msg->bytes);
    oled.update(*param);
  }
}
/// @brief OLED - エフェクターパラメータ一選択表示
/// @param[in] oled OLED通信オブジェクト
/// @param[in] msg リクエストメッセージ
void oledSelectEffecorParam(satoh::SSD1306 &oled, satoh::Message const *msg)
{
  if (oled.ok())
  {
    auto *param = reinterpret_cast<satoh::msg::OLED_SELECT_PARAM const *>(msg->bytes);
    oled.update(*param);
  }
}
/// @brief OLED - バンク画面表示
/// @param[in] oled OLED通信オブジェクト
/// @param[in] msg リクエストメッセージ
void oledDispBank(satoh::SSD1306 &oled, satoh::Message const *msg)
{
  if (oled.ok())
  {
    auto *param = reinterpret_cast<satoh::msg::OLED_DISP_BANK const *>(msg->bytes);
    oled.update(*param);
  }
}
/// @brief OLED - エフェクターパラメータ一値更新
/// @param[in] oled OLED通信オブジェクト
void oledUpdateEffectorParam(satoh::SSD1306 &oled)
{
  if (oled.ok())
  {
    oled.updateParam();
  }
}
} // namespace

void i2cTaskProc(void const *argument)
{
  if (satoh::registerMsgTarget(12) != osOK)
  {
    return;
  }
  s_i2c = new satoh::I2C(I2C1, i2cTaskHandle, DMA1, LL_DMA_STREAM_0, LL_DMA_STREAM_7);
  initPCM3060(s_i2c);
  satoh::SSD1306 oled(s_i2c);
  satoh::PCA9635 led(s_i2c);
  satoh::LevelMeter level(s_i2c);
  satoh::RotaryEncoder encoder(s_i2c);
  satoh::AT42QT1070 modeKey(s_i2c);
  satoh::Gyro mpu6050(s_i2c, satoh::MPU6050);
  satoh::Gyro icm20602(s_i2c, satoh::ICM20602);
  for (;;)
  {
    auto res = satoh::recvMsg();
    if (res.status() != osOK)
    {
      continue;
    }
    auto *msg = res.msg();
    if (msg == 0)
    {
      continue;
    }
    switch (msg->type)
    {
    case satoh::msg::LED_LEVEL_UPDATE_REQ:
      ledLevelUpdateProc(level, msg);
      break;
    case satoh::msg::LED_SIMPLE_REQ:
      ledSimpleProc(level, msg);
      break;
    case satoh::msg::LED_EFFECT_REQ:
      ledEffectProc(led, msg);
      break;
    case satoh::msg::LED_ALL_EFFECT_REQ:
      ledAllEffectProc(led, msg);
      break;
    case satoh::msg::GYRO_GET_REQ:
      res.reset();
      gyroGetProc(mpu6050, icm20602);
      break;
    case satoh::msg::MODE_KEY_GET_REQ:
      res.reset();
      keyUpdateProc(modeKey);
      break;
    case satoh::msg::ENCODER_GET_REQ:
      res.reset();
      encoderGetProc(encoder);
      break;
    case satoh::msg::OLED_DISP_EFFECTOR_REQ:
      oledDispEffector(oled, msg);
      break;
    case satoh::msg::OLED_SELECT_PARAM_REQ:
      oledSelectEffecorParam(oled, msg);
      break;
    case satoh::msg::OLED_DISP_BANK_REQ:
      oledDispBank(oled, msg);
      break;
    case satoh::msg::OLED_UPDATE_PARAM_REQ:
      res.reset();
      oledUpdateEffectorParam(oled);
      break;
    }
  }
}

void i2cEvIRQ(void)
{
  s_i2c->notifyEvIRQ();
}

void i2cErIRQ(void)
{
  s_i2c->notifyErIRQ();
}

void i2cRxEndIRQ(void)
{
  s_i2c->notifyRxEndIRQ();
}

void i2cRxErrorIRQ(void)
{
  s_i2c->notifyRxErrorIRQ();
}

void i2cTxEndIRQ(void)
{
  s_i2c->notifyTxEndIRQ();
}

void i2cTxErrorIRQ(void)
{
  s_i2c->notifyTxErrorIRQ();
}

void extiSwIRQ(void)
{
  satoh::sendMsg(i2cTaskHandle, satoh::msg::ENCODER_GET_REQ);
  notifyExtiSwIRQ();
}

void extiSw2IRQ(void)
{
  satoh::sendMsg(i2cTaskHandle, satoh::msg::MODE_KEY_GET_REQ);
  notifyExtiSw2IRQ();
}

void extiMpuIRQ(void)
{
  satoh::sendMsg(i2cTaskHandle, satoh::msg::GYRO_GET_REQ);
}
