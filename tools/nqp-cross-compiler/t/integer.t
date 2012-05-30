#!nqp
use MASTTesting;

plan(4);

mast_frame_output_is(-> $frame {
        my $r0 := MAST::Local.new(:index($frame.add_local(int)));
        my @ins := $frame.instructions;
        nqp::push(@ins, MAST::Op.new(
                :bank('primitives'), :op('const_i64'),
                $r0,
                MAST::IVal.new( :value(101) )
            ));
        nqp::push(@ins, MAST::Op.new(
                :bank('dev'), :op('say_i'),
                $r0
            ));
        nqp::push(@ins, MAST::Op.new( :bank('primitives'), :op('return') ));
    },
    "101\n",
    "integer constant loading");

mast_frame_output_is(-> $frame {
        my $r0 := MAST::Local.new(:index($frame.add_local(int)));
        my $r1 := MAST::Local.new(:index($frame.add_local(int)));
        my $r2 := MAST::Local.new(:index($frame.add_local(int)));
        my @ins := $frame.instructions;
        nqp::push(@ins, MAST::Op.new(
                :bank('primitives'), :op('const_i64'),
                $r0,
                MAST::IVal.new( :value(34) )
            ));
        nqp::push(@ins, MAST::Op.new(
                :bank('primitives'), :op('const_i64'),
                $r1,
                MAST::IVal.new( :value(8) )
            ));
        nqp::push(@ins, MAST::Op.new(
                :bank('primitives'), :op('add_i'),
                $r2, $r0, $r1
            ));
        nqp::push(@ins, MAST::Op.new(
                :bank('dev'), :op('say_i'),
                $r2
            ));
        nqp::push(@ins, MAST::Op.new( :bank('primitives'), :op('return') ));
    },
    "42\n",
    "integer addition");

mast_frame_output_is(-> $frame {
        my $r0 := MAST::Local.new(:index($frame.add_local(int)));
        my $r1 := MAST::Local.new(:index($frame.add_local(int)));
        my $r2 := MAST::Local.new(:index($frame.add_local(int)));
        my @ins := $frame.instructions;
        nqp::push(@ins, MAST::Op.new(
                :bank('primitives'), :op('const_i64'),
                $r0,
                MAST::IVal.new( :value(34) )
            ));
        nqp::push(@ins, MAST::Op.new(
                :bank('primitives'), :op('const_i64'),
                $r1,
                MAST::IVal.new( :value(8) )
            ));
        nqp::push(@ins, MAST::Op.new(
                :bank('primitives'), :op('sub_i'),
                $r2, $r0, $r1
            ));
        nqp::push(@ins, MAST::Op.new(
                :bank('dev'), :op('say_i'),
                $r2
            ));
        nqp::push(@ins, MAST::Op.new( :bank('primitives'), :op('return') ));
    },
    "26\n",
    "integer subtraction");

mast_frame_output_is(-> $frame {
        my $r0 := MAST::Local.new(:index($frame.add_local(int)));
        my $r1 := MAST::Local.new(:index($frame.add_local(int)));
        my $r2 := MAST::Local.new(:index($frame.add_local(int)));
        my @ins := $frame.instructions;
        nqp::push(@ins, MAST::Op.new(
                :bank('primitives'), :op('const_i64'),
                $r0,
                MAST::IVal.new( :value(10) )
            ));
        nqp::push(@ins, MAST::Op.new(
                :bank('primitives'), :op('const_i64'),
                $r1,
                MAST::IVal.new( :value(5) )
            ));
        nqp::push(@ins, MAST::Op.new(
                :bank('primitives'), :op('mul_i'),
                $r2, $r0, $r1
            ));
        nqp::push(@ins, MAST::Op.new(
                :bank('dev'), :op('say_i'),
                $r2
            ));
        nqp::push(@ins, MAST::Op.new( :bank('primitives'), :op('return') ));
    },
    "50\n",
    "integer multiplication");