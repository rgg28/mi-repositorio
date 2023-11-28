#pragma once
#include <map>
#include <array>

#include <fuji/fuji_common.h>
#include <eeiv/ee_fuji.h>

#define IvFujiSuperAsm(op) IvFujiOp(MipsIVInterpreter::op)

namespace cosmic::fuji {
    struct OutOfOrder {
        enum EffectivePipeline {
            InvalidOne = 0,
            Eret = 0x10,
            Cop0 = 0x12
        };
        friend EffectivePipeline operator^(EffectivePipeline dest, EffectivePipeline src) {
            return static_cast<EffectivePipeline>(static_cast<u16>(dest) ^ static_cast<u16>(src));
        }
    };

    struct InvokeOpInfo {
        Operands ops;
        OutOfOrder::EffectivePipeline pipe;
        std::function<void(InvokeOpInfo& info)> execute;
    };
    struct CachedMultiOp {
        u16 trackIndex;
        u32 trackablePC;
        InvokeOpInfo infoCallable;
    };

    struct CachedBlock {
        std::array<CachedMultiOp, 128> ops;
    };

    struct BlockFrequencyMetric {
        u32 blockPC;
        u32 heat;
        bool isLoaded;

        bool operator<(const BlockFrequencyMetric& src) const {
            return heat < src.heat;
        }
    };

    class MipsIVInterpreter : public eeiv::EEExecutor {
    public:
        static constexpr u32 superBlockCount{0x80};
        MipsIVInterpreter(eeiv::EEMipsCore& mips);
        u32 executeCode() override;
    private:
        void runFasterBlock(u32 pc, u32 block);
        u32 runNestedBlocks(std::span<CachedMultiOp> run);

        std::unique_ptr<CachedBlock> translateBlock(std::unique_ptr<CachedBlock> translated, u32 nextPC);

        u32 fetchPcInst() override;

        std::function<void(InvokeOpInfo&)> decMipsIvS(u32 opcode, InvokeOpInfo& decode);
        std::function<void(InvokeOpInfo&)> decMipsIvRegImm(u32 opcode, InvokeOpInfo& decode);
        std::function<void(InvokeOpInfo&)> decMipsIvCop0(u32 opcode, InvokeOpInfo& decode);

        InvokeOpInfo decMipsBlackBox(u32 opcode);
        void performOp(InvokeOpInfo& func, bool deduceCycles = true);

        std::array<BlockFrequencyMetric, 16> metrics;
        std::map<u32, std::unique_ptr<CachedBlock>> cached;
        u32 lastCleaned;

        IvFujiOp(addi);
        IvFujiOp(slti);
        IvFujiOp(sw); IvFujiOp(sd);
        IvFujiOp(bltzal);
        IvFujiOp(bgez); IvFujiOp(bgezl); IvFujiOp(bgezall);
        IvFujiOp(mtsab); IvFujiOp(mtsah);
        IvFujiOp(ivSyscall);

        // Memory read functions through direct translation
        IvFujiOp(lb); IvFujiOp(lbu);
        IvFujiOp(lh); IvFujiOp(lhu);
        IvFujiOp(lw); IvFujiOp(lwu);
        IvFujiOp(ld);

        IvFujiOp(movz); IvFujiOp(movn);

        IvFujiOp(cache);
        IvFujiOp(nop);
        IvFujiOp(ivBreak);

        IvFujiOp(sll); IvFujiOp(srl); IvFujiOp(sra);
        IvFujiOp(sllv); IvFujiOp(srlv); IvFujiOp(srav);

        IvFujiOp(mult); IvFujiOp(multu);
        IvFujiOp(div); IvFujiOp(divu);
        IvFujiOp(add); IvFujiOp(addu); IvFujiOp(dadd); IvFujiOp(daddu);
        IvFujiOp(sub); IvFujiOp(subu); IvFujiOp(dsub); IvFujiOp(dsubu);

        IvFujiOp(slt);
        IvFujiOp(ivXor);

        // Instructions intrinsically related to Cop0 and TLB/Exception
        IvFujiOp(tlbr);
        IvFujiOp(tlbwi);
        IvFujiOp(eret);
        IvFujiOp(ei); IvFujiOp(di);

        IvFujiOp(c0mfc);
        IvFujiOp(c0mtc);
        IvFujiOp(copbc0tf);
    };
}
