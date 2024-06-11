#!/bin/bash

#-i, --interactive: Run the program in interactive mode, allowing users to engage in real-time conversations or provide specific instructions to the model.
#--interactive-first: Run the program in interactive mode and immediately wait for user input before starting the text generation.
#-ins, --instruct: Run the program in instruction mode, which is specifically designed to work with Alpaca models that excel in completing tasks based on user instructions.

#-c N, --ctx-size N: Set the size of the prompt context (default: 512). The LLaMA models were built with a context of 2048, which will yield the best results on longer input/# inference. However, increasing the context size beyond 2048 may lead to unpredictable results.

#--keep N: Specify the number of tokens from the initial prompt to retain when the model resets its internal context. By default, this value is set to 0 (meaning no tokens # are kept). Use -1 to retain all tokens from the initial prompt.

#     -n N, --n-predict N: Set the number of tokens to predict when generating text (default: 128, -1 = infinity, -2 = until context filled)
#If the pause is undesirable, a value of -2 will stop generation immediately when the context is filled.

# -b N, --batch-size N: Set the batch size for prompt processing (default: 2048). This large batch size benefits users who have BLAS installed and enabled it during the 
# build. If you don't have BLAS enabled ("BLAS=0"), you can use a smaller number, such as 8, to see the prompt progress as it's evaluated in some situations.

cd `dirname $0`
cd ..
    
    #-r "User:" -f prompts/chat-with-bob.txt
    
    
  /home/red/Documents/BUP_SRC/WORK/Projects/llama.cpp/main \
  -m /home/red/Documents/BUP_SRC/WORK/Projects/llama.cpp/models/llama-13b/firefly-llama2-13b-chat.Q6_K.gguf \
  -c 2048 \
  -b 2048 \
  -n -2 \
  --keep -1 \
  --repeat_penalty 1.0 \
  --color \
  --interactive-first \
  | tee /home/red/Documents/BUP_SRC/WORK/Projects/llama.cpp/output.txt  
    
