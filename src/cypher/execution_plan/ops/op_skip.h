﻿/* Copyright (c) 2022 AntGroup. All Rights Reserved. */

//
// Created by wt on 2020/5/15.
//
#pragma once

#include "op.h"

namespace cypher {

class Skip : public OpBase {
    size_t rec_to_skip_ = 0;
    size_t skipped_ = 0;

 public:
    Skip(size_t rec_to_skip) : OpBase(OpType::SKIP, "Skip"), rec_to_skip_(rec_to_skip) {}

    OpResult Initialize(RTContext *ctx) override {
        CYPHER_THROW_ASSERT(!children.empty());
        auto child = children[0];
        auto res = child->Initialize(ctx);
        if (res != OP_OK) return res;
        record = child->record;
        return OP_OK;
    }

    OpResult RealConsume(RTContext *ctx) override {
        auto child = children[0];
        // As long as we're required to skip
        while (skipped_ < rec_to_skip_) {
            auto res = child->RealConsume(ctx);
            if (res != OP_OK) return res;
            skipped_++;
        }
        return child->RealConsume(ctx);
    }

    OpResult ResetImpl(bool complete) override {
        skipped_ = 0;
        return OP_OK;
    }

    std::string ToString() const override {
        std::string str(name);
        str.append(" [").append(std::to_string(rec_to_skip_)).append("]");
        return str;
    }

    CYPHER_DEFINE_VISITABLE()

    CYPHER_DEFINE_CONST_VISITABLE()

};

}  // namespace cypher
