#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define rows 8
#define cols 4
#define INPUT_COUNT 3
#define HIDDEN_COUNT 3
#define OUTPUT_COUNT 1
#define LEARNING_RATE 0.15

typedef struct
{
  float in_Weights[INPUT_COUNT];
  float inBias;
  float value;
  float out_Weights[OUTPUT_COUNT];
} Neuron;

typedef struct
{
  float value;
} IO_Neuron;

IO_Neuron trainingData[rows][cols] = {
    {{1}, {0}, {1}, {0}},
    {{1}, {0}, {0}, {0}},
    {{1}, {1}, {0}, {1}},
    {{1}, {1}, {1}, {1}},
    {{0}, {1}, {0}, {1}},
    {{0}, {1}, {1}, {1}},
    {{0}, {0}, {1}, {0}},
    {{0}, {0}, {0}, {0}}
};

typedef struct
{
  int success;
  IO_Neuron **training_in;
  IO_Neuron **training_out;
  int examples;
} TData;

TData tData()
{
  int count = 7;
  TData ret;
  ret.success = 1;
  int i, j;
  ret.training_in = malloc(sizeof(IO_Neuron *) * count);
  ret.training_out = malloc(sizeof(IO_Neuron *) * count);
  ret.examples = count;

  for (i = 0; i < count; i++)
  {
    ret.training_in[i] = malloc(sizeof(IO_Neuron) * INPUT_COUNT);
  }
  for (i = 0; i < count; i++)
  {
    ret.training_out[i] = malloc(sizeof(IO_Neuron) * OUTPUT_COUNT);
  }
  for (i = 0; i < count; i++)
  {
    int inIndex = 0;
    int outIndex = 0;
    for (j = 0; j < cols - 1; j++)
    {
      ret.training_in[i][inIndex] = trainingData[i][j];
      inIndex += 1;
    }
    ret.training_out[i][outIndex] = trainingData[i][j];
    outIndex += 1;
  }
  return ret;
}

float genRandRange(float min, float max)
{
  if (min == max)
    return min;
  float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
  return min + scale * (max - min);       /* [min, max] */
}

double power(double base, int exponent)
{
  double result = 1.0;
  for (int i = 0; i < exponent; i++)
  {
    result *= base;
  }
  return result;
}

double factorial(int n)
{
  double result = 1.0;
  for (int i = 1; i <= n; i++)
  {
    result *= i;
  }
  return result;
}

double custom_exp(double x, int n)
{
  double result = 0.0;
  for (int i = 0; i < n; i++)
  {
    result += power(x, i) / factorial(i);
  }
  return result;
}

float sigmoid(float x)
{
  return custom_exp(x, 20) / (1 + custom_exp(x, 20));
}

float sigmoid_derivative(float x)
{
  return sigmoid(x) * (1 - sigmoid(x));
}

float dot_summation(float *in, float *weights, int count)
{
  int i;
  float result = 0;
  for (i = 0; i < count; i++)
  {
    result += in[i] * weights[i];
  }
  return result;
}

float *ioValues(IO_Neuron *hidden_layer)
{
  float *ret = malloc(sizeof(float) * INPUT_COUNT);
  int i;
  for (i = 0; i < INPUT_COUNT; i++)
  {
    ret[i] = hidden_layer[i].value;
  }
  return ret;
}

float *values(Neuron *hidden_layer)
{
  float *ret = malloc(sizeof(float) * HIDDEN_COUNT);
  int i;
  for (i = 0; i < HIDDEN_COUNT; i++)
  {
    ret[i] = hidden_layer[i].value;
  }
  return ret;
}

float *outWeights(Neuron *hidden_layer, int index)
{
  float *ret = malloc(sizeof(float) * HIDDEN_COUNT);
  int i;
  for (i = 0; i < HIDDEN_COUNT; i++)
  {
    ret[i] = hidden_layer[i].out_Weights[index];
  }
  return ret;
}

void think(IO_Neuron *input_layer, Neuron *hidden_layer, IO_Neuron *output_layer)
{
  int i;
  float *io_values = ioValues(input_layer);
  for (i = 0; i < HIDDEN_COUNT; i++)
  {
    hidden_layer[i].value = sigmoid(dot_summation(io_values, hidden_layer[i].in_Weights, INPUT_COUNT) + hidden_layer[i].inBias);
  }
  free(io_values);
  float *hidden_values = values(hidden_layer);
  for (i = 0; i < OUTPUT_COUNT; i++)
  {
    float *out_weights = outWeights(hidden_layer, i);
    output_layer[i].value = sigmoid(dot_summation(hidden_values, out_weights, HIDDEN_COUNT));
    free(out_weights);
  }
  free(hidden_values);
}

void train(IO_Neuron *input_layer, Neuron *hidden_layer, IO_Neuron *output_layer, IO_Neuron **input_training, IO_Neuron **output_training, int training_samples, int iterations)
{
  int i, j, k, l;
  IO_Neuron recorded_outputs[training_samples][OUTPUT_COUNT];
  Neuron recorded_hidden[training_samples][HIDDEN_COUNT];
  float error_output[training_samples][OUTPUT_COUNT];
  float error_hidden[training_samples][HIDDEN_COUNT];
  for (i = 0; i < iterations; i++)
  {
    for (j = 0; j < training_samples; j++)
    {
      think(input_training[j], hidden_layer, output_layer);
      memcpy(recorded_outputs[j], output_layer, sizeof(IO_Neuron) * OUTPUT_COUNT);
      memcpy(recorded_hidden[j], hidden_layer, sizeof(Neuron) * HIDDEN_COUNT);
    }
    for (j = 0; j < training_samples; j++)
    {
      for (k = 0; k < OUTPUT_COUNT; k++)
      {
        error_output[j][k] = recorded_outputs[j][k].value * (1 - recorded_outputs[j][k].value) * (output_training[j][k].value - recorded_outputs[j][k].value);
      }
    }
    for (j = 0; j < training_samples; j++)
    {
      for (k = 0; k < HIDDEN_COUNT; k++)
      {
        float errorFactor = 0;
        for (l = 0; l < OUTPUT_COUNT; l++)
        {
          errorFactor += (error_output[j][l] * hidden_layer[k].out_Weights[l]);
        }
        error_hidden[j][k] = recorded_hidden[j][k].value * (1 - recorded_hidden[j][k].value) * errorFactor;
      }
    }
    for (j = 0; j < training_samples; j++)
    {
      for (k = 0; k < HIDDEN_COUNT; k++)
      {
        hidden_layer[k].inBias = hidden_layer[k].inBias + LEARNING_RATE * error_hidden[j][k];
        for (l = 0; l < INPUT_COUNT; l++)
        {
          hidden_layer[k].in_Weights[l] = hidden_layer[k].in_Weights[l] + (LEARNING_RATE * error_hidden[j][k] * input_training[j][l].value) / training_samples;
        }
      }
    }
    for (j = 0; j < training_samples; j++)
    {
      for (k = 0; k < HIDDEN_COUNT; k++)
      {
        for (l = 0; l < OUTPUT_COUNT; l++)
        {
          hidden_layer[k].out_Weights[l] = hidden_layer[k].out_Weights[l] + (LEARNING_RATE * error_output[j][k] * recorded_hidden[j][k].value) / training_samples;
        }
      }
    }
  }
}

void randweights(Neuron *neurons)
{
  int i;
  for (i = 0; i < HIDDEN_COUNT; i++)
  {
    neurons[i].in_Weights[0] = 2 * genRandRange(0, 1) - 1;
    neurons[i].in_Weights[1] = 2 * genRandRange(0, 1) - 1;
    neurons[i].in_Weights[2] = 2 * genRandRange(0, 1) - 1;
    neurons[i].out_Weights[0] = 2 * genRandRange(0, 1) - 1;
    neurons[i].inBias = 2 * genRandRange(0, 1) - 1;
  }
}

int main()
{
    clock_t start, end;
    srand(1);
    int i, j;
    TData t_data = tData();
    if (!t_data.success)
    {
        return 0;
    }
    IO_Neuron **training_in = t_data.training_in;
    IO_Neuron **training_out = t_data.training_out;

    Neuron *hidden_layer = malloc(sizeof(Neuron) * HIDDEN_COUNT);
    randweights(hidden_layer);    

    for(i = 0; i<HIDDEN_COUNT; i++)
    {
        printf("%f; ", hidden_layer[i].in_Weights[0]);
        printf("%f; ", hidden_layer[i].in_Weights[1]);
        printf("%f; ", hidden_layer[i].in_Weights[2]);
        printf("%f; ", hidden_layer[i].out_Weights[0]);
        printf("%f; \n", hidden_layer[i].inBias);
    }

    start = clock();

    for (i = 0; i < t_data.examples; i++)
    {
        think(training_in[i], hidden_layer, training_out[i]);
        for (j = 0; j < OUTPUT_COUNT; j++)
        {
            printf("%f ", training_out[i][j].value);
        }
        printf("\n");
    }    

    end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Execution Time: %f seconds\n", time_taken);

    for (i = 0; i < t_data.examples; i++)
    {
        free(training_in[i]);
        free(training_out[i]);
    }

    free(training_in);
    free(training_out);    
    free(hidden_layer);

    return 0;
}

