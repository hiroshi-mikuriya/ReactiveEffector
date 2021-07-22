#pragma once

#include "constant.h"
#include <memory>

/* ディレイバッファ 16ビット整数 -------------------------------------*/
class delayBuf
{
  std::unique_ptr<int16_t> delayArray; // ディレイバッファ配列
  uint32_t wpos = 0;                   // write position 書き込み位置
  uint32_t maxDelaySample = 1;         // 最大ディレイタイム サンプル数

public:
  /// @param[in] maxDelayTime バッファ配列メモリ確保 最大ディレイ時間 ms
  explicit delayBuf(float maxDelayTime) //
      : delayArray(new int16_t[1 + static_cast<uint32_t>(satoh::SAMPLING_FREQ * maxDelayTime / 1000.0f)])
  {
  }

  void write(float x) // バッファ配列書き込み、書込位置を進める
  {
    if (x < -1.00f)
      x = -1.00f; // オーバーフロー防止
    if (x > 0.999f)
      x = 0.999f;
    delayArray.get()[wpos] = (int16_t)(x * 32768.0f); // 16ビット整数で書き込み
    wpos++;                                           // 書込位置を1つ進める
    if (wpos == maxDelaySample)
      wpos = 0; // 最大サンプル数までで0に戻す
  }

  float read(float delayTime) // 通常のサンプル単位での読み出し ディレイ時間 ms
  {
    uint32_t interval = (uint32_t)(0.001f * delayTime * satoh::SAMPLING_FREQ); // 書込位置と読出位置の間隔を計算
    if (interval > maxDelaySample)
      interval = maxDelaySample; // 最大ディレイ時間を超えないようにする
    uint32_t rpos;               // read position 読出位置
    if (wpos >= interval)
      rpos = wpos - interval; // 読出位置を取得
    else
      rpos = wpos + maxDelaySample - interval;
    return (float)delayArray.get()[rpos] / 32768.0f; // バッファ配列からfloat(-1～1)で読み出し
  }

  float readLerp(float delayTime) // 線形補間して読み出し コーラス等に利用
  {
    float intervalF = 0.001f * delayTime * satoh::SAMPLING_FREQ; // 書込位置と読出位置の間隔を計算 float
    if (intervalF > (float)maxDelaySample)
      intervalF = (float)maxDelaySample; // 最大ディレイ時間を超えないようにする
    float rposF;                         // read position 読出位置 float
    if ((float)wpos >= intervalF)
      rposF = (float)wpos - intervalF; // 読出位置を取得 float
    else
      rposF = (float)(wpos + maxDelaySample) - intervalF;
    uint32_t rpos0 = (uint32_t)rposF; // 読出位置の前後の整数値を取得
    uint32_t rpos1 = rpos0 + 1;
    if (rpos1 == maxDelaySample)
      rpos1 = 0;                               // 読み出し位置最大と0にまたがる場合
    float y0 = (float)delayArray.get()[rpos0]; // 読出位置の前後のデータ 線形補間用一時変数
    float y1 = (float)delayArray.get()[rpos1];
    return (y0 + (rposF - (float)rpos0) * (y1 - y0)) / 32768.0f; // 線形補間値をfloat(-1～1)で読み出し
  }

  float readFixed() // 固定時間（最大ディレイタイム）で読み出し
  {
    return (float)delayArray.get()[wpos] / 32768.0f; // バッファ配列からfloat(-1～1)で読み出し
  }
};

/* ディレイバッファ 32ビット float -------------------------------------*/
class delayBufF
{
  /// @brief デフォルトコンストラクタ削除
  delayBufF() = delete;
  /// @brief コピーコンストラクタ削除
  delayBufF(delayBufF const &) = delete;
  /// @brief コピー演算子削除
  delayBufF &operator=(delayBufF const &) = delete;

  /// @brief 書込位置と読出位置の間隔を計算
  static constexpr float getInterval(float time) { return 0.001f * time * satoh::SAMPLING_FREQ; }
  /// @brief バッファサイズ取得
  /// @param[in] time 最大保持時間（ミリ秒）
  /// @return バッファサイズ
  static constexpr uint32_t getBufferSize(float time) { return 1 + static_cast<uint32_t>(getInterval(time)); }

  uint32_t maxDelaySample_;    // 最大ディレイタイム サンプル数
  std::unique_ptr<float> buf_; // ディレイバッファ配列 float
  uint32_t wpos_;              // write position 書き込み位置

public:
  /// @brief コンストラクタ
  /// @param[in] maxTime 最大保持時間（ミリ秒）
  explicit delayBufF(float maxTime) noexcept     //
      : maxDelaySample_(getBufferSize(maxTime)), //
        buf_(new float[maxDelaySample_]),        //
        wpos_(0)                                 //
  {
    memset(buf_.get(), 0, maxDelaySample_ * sizeof(float));
  }
  /// @brief moveコンストラクタ
  /// @param[in] that 移動元
  explicit delayBufF(delayBufF &&that) noexcept //
      : maxDelaySample_(that.maxDelaySample_),  //
        buf_(std::move(that.buf_)),             //
        wpos_(that.wpos_)                       //
  {
  }
  /// @brief move演算子
  /// @param[in] that 移動元
  /// @return 自身の参照
  delayBufF &operator=(delayBufF &&that) noexcept
  {
    if (this != &that)
    {
      maxDelaySample_ = that.maxDelaySample_;
      buf_ = std::move(that.buf_);
      wpos_ = that.wpos_;
    }
    return *this;
  }
  /// @brief デストラクタ
  virtual ~delayBufF() {}
  /// @brief バッファ配列書き込み、書込位置を進める
  /// @param[in] v 書き込むデータ
  void write(float v) noexcept
  {
    buf_.get()[wpos_] = v;
    wpos_ = (wpos_ + 1) % maxDelaySample_;
  }
  /// @brief 通常のサンプル単位での読み出し
  /// @param[in] delayTime ディレイ時間 ms
  float read(float delayTime) const noexcept
  {
    uint32_t interval = static_cast<uint32_t>(getInterval(delayTime));
    interval = std::min(interval, maxDelaySample_);
    uint32_t rpos = interval <= wpos_                        //
                        ? wpos_ - interval                   //
                        : wpos_ + maxDelaySample_ - interval //
        ;
    return buf_.get()[rpos];
  }
  /// @brief 線形補間して読み出し（コーラス等に利用）
  /// @param[in] delayTime ディレイ時間 ms
  float readLerp(float delayTime) const noexcept
  {
    float intervalF = std::min(getInterval(delayTime), static_cast<float>(maxDelaySample_));
    float rposF = intervalF <= wpos_                        //
                      ? wpos_ - intervalF                   //
                      : wpos_ + maxDelaySample_ - intervalF //
        ;
    uint32_t rpos0 = static_cast<uint32_t>(rposF);
    uint32_t rpos1 = (rpos0 + 1) % maxDelaySample_;
    float t = rposF - rpos0;
    return (1 - t) * buf_.get()[rpos0] + t * buf_.get()[rpos1];
  }
  /// @brief 固定時間（最大ディレイタイム）で読み出し
  float readFixed() const noexcept { return buf_.get()[wpos_]; }
};
