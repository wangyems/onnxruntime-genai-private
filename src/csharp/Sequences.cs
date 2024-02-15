// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Runtime.InteropServices;

namespace Microsoft.ML.OnnxRuntimeGenAI
{
    public class Sequences : IDisposable
    {
        private IntPtr _sequencesHandle;
        private bool _disposed = false;
        private ulong _numSequences;

        internal Sequences(IntPtr sequencesHandle)
        {
            _sequencesHandle = sequencesHandle;
            _numSequences = NativeMethods.OgaSequencesCount(_sequencesHandle).ToUInt64();
        }

        internal IntPtr Handle { get { return _sequencesHandle; } }

        public ulong NumSequences { get { return _numSequences; } }

        public ReadOnlySpan<int> this[ulong sequenceIndex]
        {
            get
            {
                ulong sequenceLength = NativeMethods.OgaSequencesGetSequenceCount(_sequencesHandle, (UIntPtr)sequenceIndex).ToUInt64();
                IntPtr sequencePtr = NativeMethods.OgaSequencesGetSequenceData(_sequencesHandle, (UIntPtr)sequenceIndex);
                unsafe
                {
                    return new ReadOnlySpan<int>(sequencePtr.ToPointer(), (int)sequenceLength);
                }
            }
        }

        public int[][] ToArray()
        {
            int[][] sequences = new int[_numSequences][];
            for (ulong sequenceIndex = 0; sequenceIndex < _numSequences; sequenceIndex++)
            {
                sequences[sequenceIndex] = this[sequenceIndex].ToArray();
            }
            return sequences;
        }

        ~Sequences()
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
            NativeMethods.OgaDestroySequences(_sequencesHandle);
            _sequencesHandle = IntPtr.Zero;
            _disposed = true;
        }
    }
}
