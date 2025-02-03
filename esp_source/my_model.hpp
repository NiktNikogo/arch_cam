#pragma once

#include "dl_layer_base.hpp"
#include "dl_layer_model.hpp"
#include "dl_layer_conv2d.hpp"
#include "dl_layer_max_pool2d.hpp"
#include "dl_layer_reshape.hpp"
#include "dl_layer_fullyconnected.hpp"
#include <stdint.h>
#include "esp_log.h"
#include "my_model_coefficient.hpp"

using namespace dl;
using namespace layer;
using namespace my_model_coefficient;

class MyModel : public Model<int16_t> {
  private:
	Reshape<int16_t> l1;
	Conv2D<int16_t> l2;
	MaxPool2D<int16_t> l3;
	Conv2D<int16_t> l4;
	MaxPool2D<int16_t> l5;
	Conv2D<int16_t> l6;
	MaxPool2D<int16_t> l7;
	Conv2D<int16_t> l8;
	MaxPool2D<int16_t> l9;
	Reshape<int16_t> l10;
	FullyConnected<int16_t> l11;
	FullyConnected<int16_t> l12;

	int16_t output;

  public:
	MyModel() : l1(Reshape<int16_t>({160, 320, 1})),
	            l2(Conv2D<int16_t>(-7, get_sequential_1_conv2d_biasadd_filter(), get_sequential_1_conv2d_biasadd_bias(), get_sequential_1_conv2d_biasadd_activation(), PADDING_SAME_END, {2,2,2,2}, 1, 1, "l2")),
	            l3(MaxPool2D<int16_t>({4, 4}, PADDING_VALID, {}, 4, 4, "l3")),
	            l4(Conv2D<int16_t>(-7, get_sequential_1_conv2d_1_biasadd_filter(), get_sequential_1_conv2d_1_biasadd_bias(), get_sequential_1_conv2d_1_biasadd_activation(), PADDING_SAME_END, {1,1,2,2}, 1, 1, "l4")),
	            l5(MaxPool2D<int16_t>({4, 4}, PADDING_VALID, {}, 4, 4, "l5")),
	            l6(Conv2D<int16_t>(-7, get_sequential_1_conv2d_2_biasadd_filter(), get_sequential_1_conv2d_2_biasadd_bias(), get_sequential_1_conv2d_2_biasadd_activation(), PADDING_SAME_END, {1,1,1,1}, 1, 1, "l6")),
	            l7(MaxPool2D<int16_t>({4, 4}, PADDING_VALID, {}, 4, 4, "l7")),
	            l8(Conv2D<int16_t>(-8, get_sequential_1_conv2d_3_biasadd_filter(), get_sequential_1_conv2d_3_biasadd_bias(), get_sequential_1_conv2d_3_biasadd_activation(), PADDING_SAME_END, {0,0,1,1}, 1, 1, "l8")),
	            l9(MaxPool2D<int16_t>({2, 2}, PADDING_VALID, {}, 2, 2, "l9")),
	            l10(Reshape<int16_t>({1, 1, 256}, "l10")),
	            l11(FullyConnected<int16_t>(-8, get_fused_gemm_0_filter(), get_fused_gemm_0_bias(), get_fused_gemm_0_activation(), true, "l11")),
	            l12(FullyConnected<int16_t>(-8, get_fused_gemm_1_filter(), get_fused_gemm_1_bias(), NULL, true, "l12")) {}

	float execute(uint8_t *data) {
		Tensor<int16_t> input;
		input.set_exponent(-7);
		input.set_shape({160, 320, 1});
		input.malloc_element();
		for(size_t i = 0; i < 160 * 320; i++){
			int16_t *value = &input.get_element_ptr()[i];
		    *value = data[i];
			*value *= 128; // Multiplies by 2^7.
		}
		this->forward(input);
		return this->output / 256.0f; // Divides by 2^8.
	}

	void build(Tensor<int16_t> &input) {
		this->l1.build(input);
		this->l2.build(this->l1.get_output());
		this->l3.build(this->l2.get_output());
		this->l4.build(this->l3.get_output());
		this->l5.build(this->l4.get_output());
		this->l6.build(this->l5.get_output());
		this->l7.build(this->l6.get_output());
		this->l8.build(this->l7.get_output());
		this->l9.build(this->l8.get_output());
		this->l10.build(this->l9.get_output());
		this->l11.build(this->l10.get_output());
		this->l12.build(this->l11.get_output());
	}

	void call(Tensor<int16_t> &input) {
		this->l1.call(input);
		input.free_element();

		this->l2.call(this->l1.get_output());
		this->l1.get_output().free_element();

		this->l3.call(this->l2.get_output());
		this->l2.get_output().free_element();

		this->l4.call(this->l3.get_output());
		this->l3.get_output().free_element();

		this->l5.call(this->l4.get_output());
		this->l4.get_output().free_element();

		this->l6.call(this->l5.get_output());
		this->l5.get_output().free_element();

		this->l7.call(this->l6.get_output());
		this->l6.get_output().free_element();

		this->l8.call(this->l7.get_output());
		this->l7.get_output().free_element();

		this->l9.call(this->l8.get_output());
		this->l8.get_output().free_element();

		this->l10.call(this->l9.get_output());
		this->l9.get_output().free_element();

		this->l11.call(this->l10.get_output());
		this->l10.get_output().free_element();

		this->l12.call(this->l11.get_output());
		this->l11.get_output().free_element();

		output = this->l12.get_output().get_element_value(0);
		this->l12.get_output().free_element();
	}
};