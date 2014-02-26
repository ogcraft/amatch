#recordkfn = "../../../asearchdata/makarevich-litsa-fingerpoints.bin"
#samplekfn = "../../../asearchdata/makarevich-litsa-recorded-60-90sec-fingerpoints.bin"
#samplekfn = "../../../asearchdata/makarevich-litsa-recorded-9-14sec-fingerpoints.bin"
#samplekfn = "../../../asearchdata/makarevich-litsa-recorded-101-120sec-fingerpoints.bin"

#recordkfn = "../../../asearchdata/prometheus-02-eng-dvd-fingerpoints.bin"
#samplekfn = "../../../asearchdata/prometeus-record-og-44-225-268sec-fingerpoints.bin"

recordkfn = "/Users/olegg/asearchdata/mercury_rising/VTS_01_4-fingerpoints.bin"
#samplekfn = "/Users/olegg/asearchdata/mercury_rising/VTS_01_4-rec-5513-60-90sec-fingerpoints.bin"
#samplekfn = "/Users/olegg/asearchdata/mercury_rising/VTS_01_4-rec-5513-30-90sec-amp-fingerpoints.bin"
#samplekfn = "/Users/olegg/asearchdata/mercury_rising/VTS_01_4-rec-5513-06-30sec-fingerpoints.bin"
#samplekfn = "/Users/olegg/asearchdata/mercury_rising/VTS_01_4-rec-5513-14-40sec-fingerpoints.bin"
#samplekfn = "/Users/olegg/asearchdata/mercury_rising/VTS_01_4-rec-5513-170sec-amp-fingerpoints.bin"
samplekfn = "/Users/olegg/asearchdata/mercury_rising/VTS_01_4-rec-5513-30-90sec-amp-fingerpoints.bin"
#samplekfn = "/Users/olegg/asearchdata/mercury_rising/VTS_01_4-rec-5513-38-60sec-fingerpoints.bin"
#samplekfn = "/Users/olegg/asearchdata/mercury_rising/VTS_01_4-rec-5513-60-90sec-fingerpoints.bin"
#samplekfn = "/Users/olegg/asearchdata/mercury_rising/VTS_01_4-rec-5513-69sec-amp-fingerpoints.bin"
#samplekfn = "/Users/olegg/asearchdata/mercury_rising/VTS_01_4-rec-5513-69sec-fingerpoints.bin"
fullsamplekfn = "/Users/olegg/asearchdata/mercury_rising/VTS_01_4-rec-5513-fingerpoints.bin"

keys_in_sec = 86
sec_per_sample =  0.01164 

function read_keys_from_file(filename::String)
    io = open(filename)
    magic = read(io, Int32)
    n = read(io, Int32)
    a = Array(Uint32, n)
    i = 0        
    while !eof(io)
        i += 1
        a[i] = read(io, Uint32)
    end
    return a
end

function calc_dist(start_pos, record_keys, sample_keys, nsec, shift_sec)
	w = nsec * keys_in_sec
    shift = int(shift_sec * keys_in_sec)
    acc = 0
	for i = 1:w
		d =  count_ones(record_keys[start_pos+i] $ sample_keys[i+shift]) # xor
		acc = acc + d
	end	
    #println("start_pos: ", start_pos," w: ", w, " acc/w: ", acc/w)
    return w/acc
end

function calc_dist_double(start_pos, record_keys, sample_keys, nsec, shift_sec1, shift_sec2)
	w = nsec * keys_in_sec
    shift1 = int(shift_sec1 * keys_in_sec)
    shift2 = int(shift_sec2 * keys_in_sec)
    acc1 = 0
    acc2 = 0
	for i = 1:w
		d1 =  count_ones(record_keys[start_pos+i] $ sample_keys[i+shift1]) # xor
		d2 =  count_ones(record_keys[start_pos+i] $ sample_keys[i+shift2]) # xor
		acc1 += d1
        acc2 += d2
	end	
    return w/acc1, w/acc2
end

function match_old(record_fn, sample_fn, nsec)
    record_keys = read_keys_from_file(record_fn)
    sample_keys = read_keys_from_file(sample_fn)
    nrecords = length(record_keys)
    nsamples = length(sample_keys)
    @printf "Read keys: %d from: %s\n"  nrecords  record_fn
    @printf "Read keys: %d from: %s\n" nsamples sample_fn
    nrecords = 10000
    diffs = fill(33, nrecords)
    diffs1 = fill(33, nrecords)
    for i = 1:(nrecords-(nsec+1) * keys_in_sec)
	    diffs[i] = calc_dist(i, record_keys, sample_keys, nsec, 0)
	    diffs1[i] = calc_dist(i, record_keys, sample_keys, nsec, 5)
    end
    @printf "Collected %d diffs\n"  length(diffs)
    m,index=findmin(diffs)
    m1,index1=findmin(diffs1)
    @printf "Found min: %d index: %d sec: %f\n\n" m index index*sec_per_sample
    @printf "Found min: %d index: %d sec: %f\n\n" m1 index1 index1*sec_per_sample
    @printf "Diff t1-t2: %f\n\n" (index1-index)*sec_per_sample
    return diffs, diffs1
end

function match1(record_keys, sample_keys, nsec, sec)
    nrecords = length(record_keys)
    nsamples = length(sample_keys)
    diffs = fill(33, nrecords)
    diffs1 = fill(33, nrecords)
    for i = 1:(nrecords-(nsec+1) * keys_in_sec)
	    diffs[i] = calc_dist(i, record_keys, sample_keys, nsec, 0)
	    diffs1[i] = calc_dist(i, record_keys, sample_keys, nsec, 5)
    end
    @printf "Collected %d diffs\n"  length(diffs)
    m,index=findmin(diffs)
    m1,index1=findmin(diffs1)
    @printf "Found sec: %f min: %d index: %d \n" index*sec_per_sample m index
    @printf "Found sec: %f min: %d index: %d \n" index1*sec_per_sample m1 index1
    diff_in_sec = (index1-index)*sec_per_sample
    @printf "Diff t1-t2: %f secs_diff: %f " diff_in_sec (sec - index * sec_per_sample)
    if abs(diff_in_sec) > 6
        @printf " ====== Not Found\n"
    else
        @printf "\n"
    end
    return diffs, diffs1
end

function match(record_fn, sample_fn, nsec)
    record_keys = read_keys_from_file(record_fn)
    sample_keys = read_keys_from_file(sample_fn)
    nrecords = length(record_keys)
    nsamples = length(sample_keys)
    @printf "Read keys: %d from: %s secs: %f\n"  nrecords  record_fn nrecords * sec_per_sample
    @printf "Read keys: %d from: %s secs: %f\n" nsamples sample_fn nsamples * sec_per_sample
    sample_size = 20 * keys_in_sec
    @printf "Sample size: %d secs: %f\n" sample_size sample_size * sec_per_sample 
    start_sample = 100
    from_end_of_record = nrecords
    i = 1
    while from_end_of_record > sample_size 
        @printf "\n---------------- Test %d -----------------\n" i
        #@printf "start_sample: %d from end: %d\n" start_sample from_end_of_record
        @printf "Looking for sec: %f (%d) from total: %f\n" start_sample * sec_per_sample start_sample nsamples * sec_per_sample

        #diff =  match_single_pass(record_keys, sample_keys[start_sample:end], nsec, start_sample * sec_per_sample)

        start_sample += sample_size
        from_end_of_record = (nsamples - 10) - (start_sample + sample_size)
        i += 1
    end
end
    
function process_diffs(in_diffs)
    avg = mean(in_diffs)
    diffs = map((x)-> x>avg ? x - avg : 0.0, in_diffs)
    m,index=findmax(diffs)
    #diffs = map((x)-> x > m/2.0 ? x : 0.0, diffs)
    return diffs, m, index
end

function match_double_pass1(track_keys, sample_keys, secs_to_match, track_ssec)
    ntrack_keys = length(track_keys)
    nsamples = length(sample_keys)
    println()
    keys_to_match = secs_to_match * keys_in_sec
    max_track_pos = ntrack_keys - keys_to_match + 1
    println("max_record_pos: ", max_track_pos)
    diffs1 = fill(0.0, max_track_pos)
    diffs2 = fill(0.0, max_track_pos)
    print("Doing: ")
    for i = 1 : max_track_pos-500
        if( i % 1000 == 0 ) print(" ", i) end
	    d1 = calc_dist(i, track_keys, sample_keys, secs_to_match, 0)
	    d2 = calc_dist(i, track_keys, sample_keys, secs_to_match, 10)
        diffs1[i] = d1
        diffs2[i] = d2
    end
    println()
    @printf "1 Collected %d diffs\n"  length(diffs1)
    m1,index1=findmax(diffs1)
    @printf "1 Found sec: %f minimum: %f index: %d \n" (track_ssec + index1 * sec_per_sample) m1 index1
    diffs1, m1, index1 = process_diffs(diffs1)
    @printf "2 Collected %d diffs\n"  length(diffs2)
    diffs2, m2, index2 = process_diffs(diffs2)
    @printf "2 Found sec: %f minimum: %f index: %d \n" (track_ssec + index2 * sec_per_sample) m2 index2
    di = index2-index1
    @printf "i2-i1 secs: %f(%d)\n" di * sec_per_sample di
    return diffs1, diffs2
end

function match_double_pass(track_keys, sample_keys, secs_to_match, track_ssec)
    ntrack_keys = length(track_keys)
    nsamples = length(sample_keys)
    println()
    keys_to_match = secs_to_match * keys_in_sec
    max_track_pos = ntrack_keys - keys_to_match + 1
    println("max_record_pos: ", max_track_pos)
    diffs1 = fill(0.0, max_track_pos)
    diffs2 = fill(0.0, max_track_pos)
    print("Doing: ")
    for i = 1 : max_track_pos-500
        if( i % 5000 == 0 ) print(" ", i) end
	    d1, d2 = calc_dist_double(i, track_keys, sample_keys, secs_to_match, 0, 10)
        diffs1[i] = d1
        diffs2[i] = d2
    end
    println()
    @printf "1 Collected %d diffs\n"  length(diffs1)
    diffs1, m1, index1 = process_diffs(diffs1)
    diffs2, m2, index2 = process_diffs(diffs2)
    sec1 =  (track_ssec + index1 * sec_per_sample)
    sec2 =  (track_ssec + index2 * sec_per_sample) 
    @printf "Found sec1: %f max1: %f i1: %d | sec2: %f max2: %f i2: %d\n" sec1 m1 index1 sec2 m2 index2 
    di = index2-index1
    ds = di * sec_per_sample
    @printf "Diff in secs: %f(%d) relative: %f\n" ds di (10-ds)/10.0
    return diffs1, diffs2
end


function match_single_pass(track_keys, sample_keys, secs_to_match, track_ssec)
    ntrack_keys = length(track_keys)
    nsamples = length(sample_keys)
    println()
    keys_to_match = secs_to_match * keys_in_sec
    max_track_pos = ntrack_keys - keys_to_match + 1
    println("max_record_pos: ", max_track_pos)
    diffs1 = fill(0.0, max_track_pos)
    diffs2 = fill(0.0, max_track_pos)
    print("Doing: ")
    for i = 1 : max_track_pos-500
        if( i % 5000 == 0 ) print(" ", i) end
	    d = calc_dist(i, track_keys, sample_keys, secs_to_match, 0)
        diffs1[i] = d
    end
    @printf "\nCollected %d diffs\n"  length(diffs1)
    diffs1, m1, index1 = process_diffs(diffs1)
    @printf "Found sec: %f minimum: %f index: %d \n" (track_ssec + index1 * sec_per_sample) m1 index1
    return diffs1, diffs2
end

function match_single_sample(track, sample, track_ssec, track_esec, sample_ssec, sample_esec, sample_sshift_secs, secs_to_match)
    @printf "match_single_sample: track_ssec: %fsec (%d) track_esec: %fsec (%d)\n" track_ssec  track_ssec * keys_in_sec track_esec  track_esec * keys_in_sec  
    @printf "match_single_sample: sample_ssec: %fsec (%d) sample_esec: %fsec (%d)\n" sample_ssec  sample_ssec * keys_in_sec sample_esec  sample_esec * keys_in_sec  
    @printf "\n--------------------------------\n" 
    sample_spos = int((sample_ssec + sample_sshift_secs) * keys_in_sec) + 1
    sample_epos = int(sample_esec * keys_in_sec)
    @printf "Looking for sec: %fsec (%d) from total: %fsec (%d)\n" sample_spos * sec_per_sample sample_spos sample_esec sample_esec * keys_in_sec
    track_spos = int(track_ssec) * keys_in_sec + 1
    track_epos = int(track_esec - 10) * keys_in_sec
    println("track_spos: ", track_spos, ", track_epos: ", track_epos)
    println("sample_spos: ", sample_spos, ", sample_epos: ", sample_epos)
    #diff1, diff2 =  match_single_pass(track[track_spos:track_epos], sample[sample_spos:sample_epos], secs_to_match, track_ssec )
    diff1, diff2 =  match_double_pass(track[track_spos:track_epos], sample[sample_spos:sample_epos], secs_to_match, track_ssec )
end


function test1()
    match(recordkfn, fullsamplekfn, 3)
end

function test_sample(nsecs_to_match)
   # try
        track_fn = "/Users/olegg/asearchdata/1/Mrsmith-5513.fpkey"
        sample_fn = "/Users/olegg/asearchdata/1/Mrsmith-rec-4500.fpkey"
        track = read_keys_from_file(track_fn)
        sample = read_keys_from_file(sample_fn)
        nrecords = length(track)
        nsamples = length(sample)
        @printf "Read keys: %d from: %s secs: %f\n"  nrecords track_fn nrecords * sec_per_sample
        @printf "Read keys: %d from: %s secs: %f\n" nsamples sample_fn nsamples * sec_per_sample
        
        match_single_sample(track, sample, 
            4400 , 4600, #(nrecords - nsamples + 1) * sec_per_sample, 
            1, 20.0, 
            0, nsecs_to_match)
    #catch e
    #    println(e)
    #end
end

