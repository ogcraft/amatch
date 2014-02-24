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
    
function match_double_pass(track_keys, sample_keys, secs_to_match, track_ssec)
    ntrack_keys = length(track_keys)
    nsamples = length(sample_keys)
    println()
    keys_to_match = secs_to_match * keys_in_sec
    max_track_pos = ntrack_keys - keys_to_match + 1
    println("max_record_pos: ", max_track_pos)
    diffs = fill(0.0, max_track_pos)
    for i = 1 : max_track_pos-500
	    d1 = calc_dist(i, track_keys, sample_keys, secs_to_match, 0)
	    d2 = calc_dist(i, track_keys, sample_keys, secs_to_match, 10)
        diffs[i] = (d1 + d2) / 2
        #if(d < 10) println("i: ", i, " d: ",d ); end
    end
    @printf "Collected %d diffs\n"  length(diffs)
    m,index=findmax(diffs)
    @printf "Found sec: %f minimum: %f index: %d \n" (track_ssec + index * sec_per_sample) m index
    return diffs
end

function match_single_pass(track_keys, sample_keys, secs_to_match, track_ssec)
    ntrack_keys = length(track_keys)
    nsamples = length(sample_keys)
    println()
    keys_to_match = secs_to_match * keys_in_sec
    max_track_pos = ntrack_keys - keys_to_match + 1
    println("max_record_pos: ", max_track_pos)
    diffs = fill(0.0, max_track_pos)
    for i = 1 : max_track_pos-500
	    d = calc_dist(i, track_keys, sample_keys, secs_to_match, 0)
        diffs[i] = d
        #if(d < 10) println("i: ", i, " d: ",d ); end
    end
    @printf "Collected %d diffs\n"  length(diffs)
    m,index=findmax(diffs)
    @printf "Found sec: %f minimum: %f index: %d \n" (track_ssec + index * sec_per_sample) m index
    return diffs
end

function match_single_sample(track, sample, track_ssec, track_esec, sample_ssec, sample_esec, sample_sshift_secs, secs_to_match)
    @printf "match_single_sample: track_ssec: %fsec (%d) track_esec: %fsec (%d)\n" track_ssec  track_ssec * keys_in_sec track_esec  track_esec * keys_in_sec  
    @printf "match_single_sample: sample_ssec: %fsec (%d) sample_esec: %fsec (%d)\n" sample_ssec  sample_ssec * keys_in_sec sample_esec  sample_esec * keys_in_sec  
    @printf "\n--------------------------------\n" 
    sample_spos = (sample_ssec + sample_sshift_secs) * keys_in_sec + 1
    sample_epos = sample_esec * keys_in_sec
    @printf "Looking for sec: %fsec (%d) from total: %fsec (%d)\n" sample_spos * sec_per_sample sample_spos sample_esec sample_esec * keys_in_sec
    track_spos = int(track_ssec) * keys_in_sec + 1
    track_epos = int(track_esec - 10) * keys_in_sec
    println("track_spos: ", track_spos, ", track_epos: ", track_epos)
    println("sample_spos: ", sample_spos, ", sample_epos: ", sample_epos)
    diff =  match_single_pass(track[track_spos:track_epos], sample[sample_spos:sample_epos], secs_to_match, track_ssec )
    #diff =  match_double_pass(track[track_spos:track_epos], sample[sample_spos:sample_epos], secs_to_match, track_ssec )
end


function test1()
    match(recordkfn, fullsamplekfn, 3)
end

function test_sample(nsecs_to_match)
   # try
        track_fn = "/Users/olegg/asearchdata/1/Mrsmith-5513.fpkey"
        sample_fn = "/Users/olegg/asearchdata/1/Mrsmith-rec-0.fpkey"
        track = read_keys_from_file(track_fn)
        sample = read_keys_from_file(sample_fn)
        nrecords = length(track)
        nsamples = length(sample)
        @printf "Read keys: %d from: %s secs: %f\n"  nrecords track_fn nrecords * sec_per_sample
        @printf "Read keys: %d from: %s secs: %f\n" nsamples sample_fn nsamples * sec_per_sample
        
        match_single_sample(track, sample, 
            0.0 , 100, #(nrecords - nsamples + 1) * sec_per_sample, 
            0, 40.0, 
            0, nsecs_to_match)
    #catch e
    #    println(e)
    #end
end

