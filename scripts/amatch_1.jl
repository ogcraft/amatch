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

function hamming_distance(strack, ssample, sz, track, sample)
	hd = 0
    for i = 1:sz
		d =  count_ones(track[i+strack] $ sample[i+ssample]) # xor
		hd += d
	end	
    return hd/sz
end

#   ti - index in track 
#   sshift - shift from begin of sample
#   msize - math window size
#   dw - distance to the next window
#   nw - num of windows
#   sz - sample size
#   track - track keys vector
#   sample - sample keys vector
#   check: sshift + nw*msize + (nw-1)*dw < sz
function match_sample(ti, sshift, msize, dw, nw, track, sample)
    sz = length(sample)
    max_sample_index =  sshift + nw*msize + (nw-1)*dw 
    i = sshift
    wcount = 0
    d = 32
    diffs = fill(33, nrecords)
    while( i < max_sample_index && i < sz )
        d = hamming_distance(ti+i, i, msize, track, sample)
        i += msize + dw
        if(wcount < nw)
            wcount += 1
        else
            break;
        end
        #println("d:", d, " d/wcount:", d/(wcount*msize), " wcount:", wcount)
    end
    return d/(wcount*msize)
end

#   ti - index in track 
#   si - start index in sample
#   msize - match window size
#   sz - sample size
#   track - track keys vector
#   sample - sample keys vector
function match_sample_simple(ti, si, msize, track, sample)
    sz = length(sample)
    max_sample_index =  si + msize 
    i = 1
    d = 32
    #println("max_sample_index:", max_sample_index, " sz:", sz)
    while( i < max_sample_index && i < sz )
        d += hamming_distance(ti+i, i, msize, track, sample)
        i += 1
        #println("d:", d, " d/i:", d/i)
    end
    #println("d:", d, " d/i:", d/i, " i:", i)
    return i/d
end

function f1(is, ie, match_sec, track, sample)
    msize = match_sec * keys_in_sec
    len = ie - is
    ds=fill(0.0, len)
    ms=fill(0.0, len)
    m = 0
    for i=1:len
        ds[i] = match_sample_simple(i+is, 1, msize, track, sample)
        m += ds[i]
        dd =  ds[i]/(m/i) 
        if(i>100)
            ms[i] = dd
        end
        if(dd > 1.5)
            #println("Found: ", i, " dd = ",dd)
            return ds, i, dd
        end
    end
    return ds, ms
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

function process_diffs1(in_diffs)
    avg = mean(in_diffs)
    diffs = map((x)-> x>avg ? x - avg : 0.0, in_diffs)
    m = 0
    index = 0
    w = 500
    for i=1:(length(diffs) - w)
        if(i<=w) 
            continue
        end
        d = diffs[i]
        dl=diffs[i-100]
        dr=diffs[i+100]
        dm = ( dl + dr )/2
        p = d - dm
        if(p > 2*dm)
            println("i:", i, "p:", p)
        end
    end
    return diffs, m, index
end

function linear_inter(x0, y0, x1, y1, x)
    return y0 + (y1-y0)*(x-x0)/(x1-x0)
end
function FWHM(X,Y)
    half_max = max(Y) / 2.0
    #find when function crosses line half_max (when sign of diff flips)
    #take the 'derivative' of signum(half_max - Y[])
    d = sign(half_max - array(Y[1:end-1])) - sign(half_max - array(Y[2:end]))
    #plot(X,d) #if you are interested
    #find the left and right most indexes
    left_idx = find(d > 0)[0]
    right_idx = find(d < 0)[-1]
    return X[right_idx] - X[left_idx] #return the difference (full width)
end


function process_diffs2(in_diffs, w)
    avg = mean(in_diffs)
    diffs = map((x)-> x>avg ? x - avg : 0.0, in_diffs)
    m = 0
    index = 0
    n = length(diffs) - 2*w - 1
    ps = fill(0.0, n)
    runm = 0
    for i=1:n
        if(i<=w) 
            continue
        end
        d = diffs[i]
        dl=diffs[i-w]
        dr=diffs[i+w]
        dm = ( dl + dr )/2
        p = d - dm
        ps[i]= abs(p)
    end
    m, index = findmax(ps)
    return ps, m, index
end

function match_single_pass(track_spos, track_epos, sample_spos, sample_epos, track, sample )
    nsamples = sample_epos - sample_spos
    ndiffs = track_epos  - track_spos - nsamples - 1
    println(" ----- match 1-------- ndiffs: ", ndiffs)
    diffs1 = fill(0.0, ndiffs)
    diffs2 = fill(0.0, ndiffs)
    print("Doing: ")
    for i = 1 : ndiffs
        if( i % 1000 == 0 ) print(" ", i) end
        #function match_sample_simple(ti, si, msize, track, sample)
        d = match_sample_simple(i, 1, 10 * keys_in_sec, 0 * keys_in_sec, 2, track, sample)
        println("i: ", i," d1: ", d);
        diffs1[i] = d
    end
    @printf "\nCollected %d diffs\n"  length(diffs1)
    diffs2, m1, index1 = process_diffs2(diffs1,100)
    @printf "Found sec: %f maximum: %f index: %d \n" (track_spos + index1) * sec_per_sample m1 index1
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
        if( i % 5000 == 0 ) print(" ", i) end
	    d1, d2 = calc_dist_double(i, track_keys, sample_keys, secs_to_match, 0, 10)
        diffs1[i] = d1
        diffs2[i] = d2
    end
    println()
    @printf "Collected %d diffs\n"  length(diffs1)
    diffs1, m1, index1 = process_diffs2(diffs1, 100)
    diffs2, m2, index2 = process_diffs2(diffs2[index1:end], 100)
    index2 += index1
    sec1 =  (track_ssec + index1 * sec_per_sample)
    sec2 =  (track_ssec + index2 * sec_per_sample) 
    @printf "Found sec1: %f max1: %f i1: %d | sec2: %f max2: %f i2: %d\n" sec1 m1 index1 sec2 m2 index2 
    di = index2-index1
    ds = di * sec_per_sample
    @printf "Diff in secs: %f(%d) relative: %f\n" ds di (10-ds)/10.0
    return diffs1, diffs2
end

function match_single_sample(track, sample, track_ssec, track_esec, sample_ssec, sample_esec, sample_sshift_secs, secs_to_match)
    @printf "match_single_sample: track_ssec: %fsec (%d) track_esec: %fsec (%d)\n" track_ssec  track_ssec * keys_in_sec track_esec  track_esec * keys_in_sec  
    @printf "match_single_sample: sample_ssec: %fsec (%d) sample_esec: %fsec (%d)\n" sample_ssec  sample_ssec * keys_in_sec sample_esec  sample_esec * keys_in_sec  
    @printf "\n--------------------------------\n" 
    sample_spos = int(sample_ssec * keys_in_sec) + 1
    sample_epos = sample_spos + int(sample_esec * keys_in_sec)
    @printf "Looking for sec: %fsec (%d) from total: %fsec (%d)\n" sample_spos * sec_per_sample sample_spos sample_esec sample_esec * keys_in_sec
    track_spos = int(track_ssec) * keys_in_sec + 1
    track_epos = int(track_esec - 10) * keys_in_sec
    println("track_spos: ", track_spos, ", track_epos: ", track_epos)
    println("sample_spos: ", sample_spos, ", sample_epos: ", sample_epos)
    #diff1, diff2 =  match_single_pass(track_spos, track_epos, sample_spos, sample_epos, track, sample )
    #diff1, diff2 =  match_double_pass(track[track_spos:track_epos], sample[sample_spos:sample_epos], secs_to_match, track_ssec )
    #diff1, diff2 =  match_double_pass1(track[track_spos:track_epos], sample[sample_spos:sample_epos], secs_to_match, track_ssec )
    ds, i, m = f1(track_spos, track_epos, secs_to_match, track, sample)
    println("=== Found sec: ", i*sec_per_sample)
    return ds
end

function test_sample(nsecs_to_match, track_fn, sample_fn)
    try
        #track_fn = "/Users/olegg/asearchdata/1/Mrsmith-5513.fpkey"
        #sample_fn = "/Users/olegg/asearchdata/1/Mrsmith-5513-6000s-fingerpoints.bin"
        track = read_keys_from_file(track_fn)
        sample = read_keys_from_file(sample_fn)
        nrecords = length(track)
        nsamples = length(sample)
        @printf "Read keys: %d from: %s secs: %f\n"  nrecords track_fn nrecords * sec_per_sample
        @printf "Read keys: %d from: %s secs: %f\n" nsamples sample_fn nsamples * sec_per_sample
        tic() 
        diffs1 = match_single_sample(track, sample, 
            0.01 , (nrecords - nsamples - 1000) * sec_per_sample, 
            0.01, 20.0, 
            0, nsecs_to_match)
        toc()
        return diffs1
    catch e
        println(e)
    end
end

