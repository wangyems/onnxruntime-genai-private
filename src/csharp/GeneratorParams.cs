// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace Microsoft.ML.OnnxRuntimeGenAI
{
    public class GeneratorParams : IDisposable
    {
        private IntPtr _generatorParamsHandle;
        private bool _disposed = false;

        public GeneratorParams(Model model)
        {
            Result.VerifySuccess(NativeMethods.OgaCreateGeneratorParams(model.Handle, out _generatorParamsHandle));
        }

        internal IntPtr Handle { get { return _generatorParamsHandle; } }

        public void SetMaxLength(ulong maxLength)
        {
            Result.VerifySuccess(NativeMethods.OgaGeneratorParamsSetMaxLength(_generatorParamsHandle, (UIntPtr)maxLength));
        }

        public void SetInputIDs(IReadOnlyCollection<int> inputIDs, ulong sequenceLength, ulong batchSize)
        {
            int[] inputIDsArray = inputIDs.ToArray();
            Result.VerifySuccess(NativeMethods.OgaGeneratorParamsSetInputIDs(_generatorParamsHandle, inputIDsArray, (UIntPtr)inputIDsArray.Length, (UIntPtr)sequenceLength, (UIntPtr)batchSize));
        }

        ~GeneratorParams()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (_disposed)
            {
                return;
            }
            NativeMethods.OgaDestroyGeneratorParams(_generatorParamsHandle);
            _generatorParamsHandle = IntPtr.Zero;
            _disposed = true;
        }
    }
}
