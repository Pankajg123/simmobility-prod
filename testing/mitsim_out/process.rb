

class Node
  def initialize(nodeID)
    @nodeID = nodeID
    @x = 0
    @y = 0
  end  

  attr_reader :nodeID
  attr_accessor :x
  attr_accessor :y
end

class Point
  def initialize(x, y)
    @x = x
    @y = y
  end  
  attr_accessor :x
  attr_accessor :y
end

#Get standard deviation and average
def getStdAvg(list)
  #Sum, average
  sum = 0
  list.each {|item|
    sum += item
  }
  avg = sum / list.length.to_f

  #Sample variance, stdev
  sVar = 0
  list.each {|item|
    sVar += (item-avg)**2
  }
  sVar /= list.length.to_f
  stDev = Math.sqrt(sVar)
  
  return [avg, stDev]
end


class Driver
  def initialize(agentID)
    @agentID = agentID
    @departure = 0
    @arrival = 0
    @completed = false
    @originNode = nil
    @destNode = nil
    @vehicleType = nil

    @firstPos = nil

    @tempVeh2 = nil #Something boolean
    @tempVeh6 = nil #Usually equals the destination node except in very rare cases (~38 of them)

    @tempVeh3 = nil #Might be distance in meters
    @tempVeh9 = nil #Same

    @tempVeh10 = nil #Almost definitely speed in m/s
    @tempVeh11 = nil #Always zero
  end  

  attr_reader :agentID
  attr_accessor :departure
  attr_accessor :arrival
  attr_accessor :completed

  attr_accessor :firstPos

  attr_accessor :originNode
  attr_accessor :destNode
  attr_accessor :vehicleType

  attr_accessor :tempVeh2
  attr_accessor :tempVeh3
  attr_accessor :tempVeh6
  attr_accessor :tempVeh9
  attr_accessor :tempVeh10
  attr_accessor :tempVeh11
end


class Segment
  def initialize(segmentID)
    @segmentID = segmentID
    @startPos = nil
    @endPos = nil
    @upNode = nil
    @downNode = nil
    @parentLink = nil
  end  

  attr_reader :segmentID
  attr_accessor :startPos
  attr_accessor :endPos
  attr_accessor :upNode
  attr_accessor :downNode
  attr_accessor :parentLink
end

class Link
  def initialize(linkID)
    @linkID = linkID
    @upNode = nil
    @downNode = nil
    @segments = [] #Listed upstream to downstream
  end  

  attr_reader :linkID
  attr_accessor :upNode
  attr_accessor :downNode
  attr_accessor :segments
end


def ParseScientificToFloat(str)
  if str =~ /([0-9]+\.[0-9]+)e([-+][0-9]+)/
    base = $1.to_f
    subscr = $2.to_f
    return base * 10.0 ** subscr
  else
    raise "String is not in scientific form: " + str
  end
end


def calc_distance(x1, y1, x2, y2)
  dX = x2 - x1
  dY = y2 - y1
  return Math.sqrt(dX**2 + dY**2)
end


#Helper; make sure all nodes exist
def attemptToGenerateID(linkID, pos, nodeID, currSubID, nodelist)
  #If no Node ID exists, generate a node and add it to the node list
  unless nodeID
    #Somewhat fragile, but should be good enough
    raise "Too many segments" if currSubID[0]>999

    #Set the ID (string)
    nodeID = "#{linkID}:#{currSubID[0]}"
    currSubID[0] += 1

    #Sanity check
    raise "Node ID collision error: #{nodeID}" if nodelist.has_key? nodeID

    #Make the node
    nd = Node.new(nodeID)
    nd.x = pos.x
    nd.y = pos.y
    nodelist[nodeID] = nd
  end

  #Return the (new?) ID
  return nodeID
end


#The network file has a complex format. Basically, we are just extracting
#  segments for now.
def read_network_file(segments, nodes)
  #First, pre-process and remove comments. Apparently *all* comment types
  #  are valid for mitsim...
  bigstring = ""
  onComment = false
  File.open('network-BUGIS.dat').each { |line|
    line.chomp!
    if onComment
      #Substitute the comment closing and set onComment to false
      if line.sub!(/^((?!\*\/).)*\*\//, '') #C style, closing
        onComment = false
      else
        line = '' #Still in the comment
      end
    end
    unless onComment
      #Single line regexes are easy
      line.sub!(/#.*$/, '')  #Perl style
      line.sub!(/\/\/.*$/, '')  #C++ style
      line.gsub!(/\/\*((?!\*\/).)*\*\//, '') #C style single line

      #Now multi-line C comments
      onComment = true if line.sub!(/\/\*((?!\*\/).)*$/, '')  #C style, line ending reached
    end
    bigstring << line
  }

  #Now, parse unrelated sections out of the "bigstring".
  bigstring.sub!(/^.*(?=\[Links\])/, '')
  bigstring.sub!(/\[(?!Links)[^\]]+\].*/, '')
  bigstring.gsub!(/[ \t]+/, ' ') #Reduce all spaces/tabs to a single space
  bigstring.gsub!(/\[Links\][^{]*{/, '') #Avoid a tiny bit of lookahead. There will be an unmatched } at the end, but it won't matter

  #Links start with:  {1 2 3 4 5   #LinkID LinkType UpNodeID DnNodeID LinkLabelID 
  posInt = '([0-9]+)'
  posDbl = '([0-9\.]+)'
  posSci = '([0-9]+\.[0-9]+e[-+][0-9]+)'
  linkHead = "{ ?#{posInt} #{posInt} #{posInt} #{posInt} #{posInt} ?"
  #After that is at least one segment: {1 2 3 4  #SegmentID DefaultSpeedLimit FreeSpeed Grad 
  segmentHead = "{ ?#{posInt} #{posInt} #{posInt} #{posInt} ?"
  #After that is exactly one descriptor: {1.0e+0 2.0e+0 3.00 4.0e+0 5.0e+0}  #StartingPntX StartingPntY Bulge EndPntX EndPntY
  segmentDesc = "{ ?#{posSci} #{posSci} #{posDbl} #{posSci} #{posSci} ?} ?"
  #After that is at least one lane rules group: {1, 2}
  laneRule = "{ ?#{posInt} #{posInt} ?} ?"
  #Close each segment with }
  segmentStr = "#{segmentHead}#{segmentDesc}((#{laneRule})+}) ?"
  segmentRegex = Regexp.new(segmentStr)
  #Close the Link with }
  linkRegex = Regexp.new("#{linkHead}((#{segmentStr})+}) ?")
  bigstring.scan(linkRegex) {|linkRes|
    linkID = linkRes[0]

    upNodeID = linkRes[2]
    downNodeID = linkRes[3]
    link = Link.new(linkID)
    link.upNode = upNodeID
    link.downNode = downNodeID

    #Now get segments
    #Segments are listed from upstream to downstream
    segmentStr = linkRes[5]
    prevEndPoint = nil
    segmentStr.scan(segmentRegex) {|segRes|
      segID = segRes[0]
      segStartX = ParseScientificToFloat(segRes[4])
      segStartY = ParseScientificToFloat(segRes[5])
      segEndX = ParseScientificToFloat(segRes[7])
      segEndY = ParseScientificToFloat(segRes[8])

      #Save it temporarily
      seg = Segment.new(segID)
      seg.startPos = Point.new(segStartX, segStartY)
      seg.endPos = Point.new(segEndX, segEndY)
      seg.parentLink = link
      link.segments.push(seg)

      #Double-check consistency
      if prevEndPoint
        unless seg.startPos.x==prevEndPoint.x and seg.startPos.y==prevEndPoint.y
          #Skip errors less than 1 m
          errorVal = calc_distance(seg.startPos.x, seg.startPos.y, prevEndPoint.x, prevEndPoint.y)
          raise "Segment consistency error on Segment: #{segID} Distance: #{errorVal}" if errorVal >= 1.0 
        end
      end
      prevEndPoint = seg.endPos
    }


    #Now set each segment's node ids and go from there
    currSubID = [1] #Node ID for segments.
    link.segments[0].upNode = upNodeID
    link.segments[-1].downNode = downNodeID

    #Force add Link start node
    id = link.segments[0].upNode
    unless nodes.has_key? id
      nd = Node.new(id)
      nd.x = link.segments[0].startPos.x
      nd.y = link.segments[0].startPos.y
      nodes[id] = nd
    else
      #Check position
      prevPos = nodes[id]
      newPos = link.segments[0].startPos
      unless prevPos.x==newPos.x and prevPos.y==newPos.y
        errorVal = calc_distance(prevPos.x, prevPos.y, newPos.x, newPos.y)
        puts "Node ID/position mismatch[#{id}],\n  prev: (#{prevPos.x},#{prevPos.y}),\n  curr: (#{newPos.x},#{newPos.y}\n  error: #{errorVal})" if errorVal >= 10.0
        puts "Node ID/position mismatch[#{id}], off by #{errorVal} meters" if errorVal >= 1.0
      end
    end

    #Force add Link end node
    id = link.segments[-1].downNode
    unless nodes.has_key? id
      nd = Node.new(id)
      nd.x = link.segments[-1].endPos.x
      nd.y = link.segments[-1].endPos.y
      nodes[id] = nd
    else
      #Check position
      prevPos = nodes[id]
      newPos = link.segments[-1].endPos
      unless prevPos.x==newPos.x and prevPos.y==newPos.y
        errorVal = calc_distance(prevPos.x, prevPos.y, newPos.x, newPos.y)
        puts "Node ID/position mismatch[#{id}],\n  prev: (#{prevPos.x},#{prevPos.y}),\n  curr: (#{newPos.x},#{newPos.y}\n  error: #{errorVal})" if errorVal >= 10.0
        puts "Node ID/position mismatch[#{id}], off by #{errorVal} meters" if errorVal >= 1.0
      end
    end

    link.segments.each{|segment|
      #Attempt to generate an ID. Even if this fails, save this ID
      segment.upNode = attemptToGenerateID(linkID, segment.startPos, segment.upNode, currSubID, nodes)
      segment.downNode = attemptToGenerateID(linkID, segment.endPos, segment.downNode, currSubID, nodes)

      #And... add it to the list
      segments[segment.segmentID.to_i] = segment
    }
  }

end



#The dep.out file contains the departure record of each vehicle in the simulation.
#This comprises of:
#    departure time, 
#    vehicle ID, 
#    origin, 
#    destination, 
#    vehicle type,
#    path
#(Actually: 5 values -1)
#  time
#  agentID
#  originNode
#  destNode
#  vehicleType
#  path, but ONLY if the vehicles have a path
def read_dep_file(list)
  min = max = nil
  File.open('dep.out').each { |line|
    if line =~ /([0-9.]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+)/
      dr = Driver.new($2.to_i)
      dr.departure = $1.to_f
      dr.originNode = $3.to_i
      dr.destNode = $4.to_i
      dr.vehicleType = $5.to_i

      unless list.has_key? dr.agentID
        list[dr.agentID] = dr
        min = dr unless min and min.agentID<=dr.agentID
        max = dr unless max and max.agentID>=dr.agentID
      else
        raise "Agent ID already exists: #{dr.agentID}"
      end
    else 
      puts "Skipped line: #{line}"
    end
  }
  return min, max
end


#The vehicle.out file contains, for vehicles that have reached their destinations, data pertaining to:
#    origin,
#    destination,
#    departure time,
#    arrival time,
#    mileage
#    speed
#(Actually: 11 values +5)
def read_veh_file(list)
  minDepartureTime = -1
  File.open('vehicle.out').each { |line|
    if line =~ /([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9.]+) +([0-9]+)/
      id = $1.to_i
      if list.has_key? id
        dr = list[id]
        raise "Agent completed twice: #{id}" if dr.completed
        dr.arrival = $8.to_i
        dr.completed = true
        raise "Origin mismatch for agent: #{id}" if dr.originNode != $4.to_i
        raise "Dest mismatch for agent: #{id}" if dr.destNode != $5.to_i
        raise "Departure time mismatch for agent: #{id}" if dr.departure.round != $7.to_i
        raise "Arrived before departure for agent: #{id}" if dr.arrival < dr.departure
        t2 = $2.to_i
        raise "Expected boolean, not: #{t2}" unless t2==0 or t2==1
        dr.tempVeh2 = t2==0 ? false : true
        dr.tempVeh3 = $3.to_i
        dr.tempVeh6 = $6.to_i
        dr.tempVeh9 = $9.to_i
        dr.tempVeh10 = $10.to_f
        t11 = $11.to_i
        raise "Expected boolean, not: #{t11}" unless t11==0 or t11==1
        dr.tempVeh11 = t11==0 ? false : true

        #Calculate min. departure time
        minDepartureTime = dr.departure if minDepartureTime==-1 or dr.departure<minDepartureTime
      else
        raise "Agent ID expected but doesn't exist: #{id}"
      end
    else 
      puts "Skipped line: #{line}"
    end
  }
  return minDepartureTime
end


#For now, this just adds firstPos value for each driver
#Total of 8 values:
#  time 
#  agentID 
#  segmentID
#  laneID 
#  positionInLane 
#  speed 
#  acceleration 
#  vehicleType
def read_traj_file(list, segments, nodes)
  lastKnownTime = 0
  unknownNodes = []
  File.open('traj_compact.txt').each { |line|  #We use a trimmed version of trajectory.out, but loading the original would work too.
    if line =~ /([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9.]+) +([0-9.]+) +([0-9e.\-]+) +([0-9]+)/
      id = $2.to_i
      if list.has_key? id
        dr = list[id]

        #Saving firstPos
        unless dr.firstPos
          dr.firstPos = $5.to_f
        end

        #Time
        time = $1.to_i
        raise "Error: time must be strictly increasing" if time<lastKnownTime

        #segmentID, laneID, positionInLane
        segmentID = $3.to_i
        raise "Segment ID not known: " + segmentID.to_s unless segments.has_key? segmentID
        laneID = $4.to_i
        posInLane = $5.to_f
        
        #Get the upstream node
        segment = segments[segmentID]
        unless segment.upNode.include? ':'  #We don't consider uni-nodes for now.
          unless nodes.has_key? segment.upNode
            unknownNodes.push(segment.upNode)
          end
        end

        #Increment
        lastKnownTime = time
      else
        raise "Agent ID expected but doesn't exist: #{id}"
      end
    else 
      puts "Skipped line: #{line}"
    end
  }


  puts "Unknown node IDs: #{unknownNodes.uniq}" unless unknownNodes.empty?

  #Build a lookup of Nodes based on the first location of each driver.
  node_lookup = {} #Array of offsets, indexed by NodeID
  list.each{|id, drv| 
    next unless drv.firstPos
    node_lookup[drv.originNode] = [] unless node_lookup.has_key? drv.originNode
    node_lookup[drv.originNode].push(drv.firstPos)
  }

  #Temp: print
  node_lookup.each {|id, offsets|
    next  #Skip for now
    min = max = offsets[0]
    puts "First 'offset' values for agents in Node ID #{id}:"
    offsets.each {|offset|
#      puts "  #{offset}"
      min = offset if offset < min
      max = offset if offset > max
    }
    avg, stddev = getStdAvg(offsets)
    puts "Average:  #{avg} +/- #{stddev}"
    puts "Min/max:  #{min} => #{max}"
    puts '-'*20
  }
end


#outputSMNodeID is just for show
def compute_error(smNodeLookup, msNode, nodeID, outputSMNodeID, offset)
  smNode = smNodeLookup[nodeID]
  return if smNode.x==0 and smNode.y==0 #Not in our lookup file

  expected = Point.new(msNode.x*100+offset.x, msNode.y*100+offset.y)
  diff = Point.new(expected.x-smNode.x, expected.y-smNode.y)
  #puts "Mitsim(#{nodeID}) to Sim Mobility(#{outputSMNodeID}) has error of: (#{diff.x},#{diff.y})"
  #puts "   mitsim: (#{msNode.x*100},#{msNode.y*100})"
  #puts "   simmob: (#{smNode.x},#{smNode.y})"
end



def read_convert_file(list, mitsimToSM, multiAndUniNodes)
  File.open('ms_sm_node_convert.txt').each { |line|
    next if line =~ /^#/ or line.strip.empty?
    if line =~ /([0-9]+) *=> *([0-9]+)/
      from = $1
      to = $2.to_i
      newNode = Node.new(to)
      if mitsimToSM.has_key? from
        raise "Comparison error" if mitsimToSM[from].nodeID != to
      else
        mitsimToSM[from] = newNode
        #multiAndUniNodes[from.to_s] = newNode
      end
    elsif line =~ /{((?:[0-9:]+[, ]*)+)} *=> *([0-9]+)/
      from_ids = $1
      to_id = $2.to_i
      newNode = Node.new(to_id)
      from_ids.scan(/[0-9:]+/) {|line|
        from_id = line
        if mitsimToSM.has_key? from_id
          raise "Comparison error" if mitsimToSM[from_id].nodeID != to_id
        else
          mitsimToSM[from_id] = newNode
        end
      }
    elsif line =~ /([0-9]+) *= *\(([0-9]+),([0-9]+)\)/
      found = false
      mitsimToSM.each_value{|nd|
        if nd.nodeID == $1.to_i
          nd.x = $2.to_i
          nd.y = $3.to_i
          found = true
        end
      }
      raise "Couldn't find node: #{$1}" unless found
    else
      puts "Skipped line: #{line}"
    end
  }

  #Final check
  mitsimToSM.each_value{|nd|
    puts "Node ID not translated: #{nd.nodeID}" if nd.x==0 or nd.y==0
  }
end


def final_validate(agents, nodeConv, segments, nodes, minDepTime)
  #Check the offset from sim mobility nodes to mitsim ones
  sampleNodePos_SM = nodeConv['122'] #122 => 60896, the lower-left most node
  sampleNodePos_MS = nil
  segments.each{|key, seg|
    if seg.upNode=='122'
      sampleNodePos_MS = seg.startPos
      break
    elsif seg.downNode=='122'
      sampleNodePos_MS = seg.endPos
      break
    end
  }
  raise "Couldn't find sample node!" unless sampleNodePos_MS

  #Offset FROM mitsim points TO sim mobility ones. (Add this to mitsim points)
  #NOTE: This doesn't work. We'll need to use the Sim Mobility coordinates directly; there's
  #      far too much error if we just scale by one of the MITSIM points. 
  offset = Point.new(sampleNodePos_SM.x-sampleNodePos_MS.x*100, sampleNodePos_SM.y-sampleNodePos_MS.y*100)
  #puts "Mitsim node location: (#{sampleNodePos_MS.x*100},#{sampleNodePos_MS.y*100})"
  #puts "Sim Mobility node location: (#{sampleNodePos_SM.x},#{sampleNodePos_SM.y})"
  #puts "Offset (ms+): (#{offset.x},#{offset.y})"

  #Now check how much error we get versus other node IDs:
  segments.each{|key, seg|
    unless seg.upNode.include? ':'
      if nodes.has_key? seg.upNode and nodeConv.has_key? seg.upNode
        compute_error(nodeConv, seg.startPos, seg.upNode, nodeConv[seg.upNode].nodeID, offset)
      end
    end
    unless seg.downNode.include? ':'
      if nodes.has_key? seg.downNode and nodeConv.has_key? seg.downNode
        compute_error(nodeConv, seg.endPos, seg.downNode, nodeConv[seg.downNode].nodeID, offset)
      end
    end
  }


  agents.each{|id, dr|
    #Make sure sim mobility node IDs exist
    raise "No Sim Mobility node id for: #{dr.originNode}" unless nodeConv.has_key? dr.originNode.to_s
    raise "No Sim Mobility node id for: #{dr.destNode}" unless nodeConv.has_key? dr.destNode.to_s
    dr.originNode = nodeConv[dr.originNode.to_s]
    dr.destNode = nodeConv[dr.destNode.to_s]

    #Start our departure times at zero, and convert to ms (then int)
    #TODO: We might want to generate a <simulation> tag instead. For now this is easier.
    dr.departure =  ((dr.departure - minDepTime)*1000).to_i
  }
end


#Generate something that the visualizer can parse.
def fakeNodeID(nodeID)
  ind = nodeID.index(':')
  return nodeID.to_i unless ind
  prefix = nodeID.slice(0,ind).to_i
  suffix = nodeID.slice(ind+1, nodeID.length).to_i
  return prefix*1000 + suffix
end


def run_main()
  #Read the network file
  segments = {}
  nodes = {}  #Note, these are MITSIM nodes, and are indexed by id as a STRING.
  read_network_file(segments, nodes)

  #TEMP:
  #nodes.each{|key, value|
  # puts "Temp: #{value.x},#{value.y} => #{key.include? ':'}"
  #}

  #Build up a list of all Driver IDs
  drivers = {}
  min, max = read_dep_file(drivers)

  #Cross-reference with the vehicles file
  minDepTime = read_veh_file(drivers)

  #Compare with sim mobility nodes
  nodeConv = {} #Lookup based on Mitsim ID (string)
  read_convert_file(drivers, nodeConv, nodes)

  #Parse the trajectory file
  read_traj_file(drivers, segments, nodes)

  #Final validation
  final_validate(drivers, nodeConv, segments, nodes, minDepTime)

  #Try printing the MITSIM node network
  knownNodeIDs = []
  allNodeIDs = []
  possibleLinks = []
  segments.each{|key, seg|
    numFound = 0
    unless seg.upNode.include? ':'
      if nodes.has_key? seg.upNode and nodeConv.has_key? seg.upNode
        knownNodeIDs.push(seg.upNode)
        numFound += 1
      end
    end
    unless seg.downNode.include? ':'
      if nodes.has_key? seg.downNode and nodeConv.has_key? seg.downNode
        knownNodeIDs.push(seg.downNode)
        numFound += 1
      end
    end

    #Hmm
    if nodes.has_key?(seg.upNode) and not allNodeIDs.include? seg.upNode
      allNodeIDs.push(seg.upNode)
    end
    if nodes.has_key?(seg.downNode) and not allNodeIDs.include? seg.downNode
      allNodeIDs.push(seg.downNode)
    end

    #Mark this link for later
    #if numFound>0
    possibleLinks.push(seg.parentLink) unless possibleLinks.include? seg.parentLink
    #end
  }
  possibleLinks.each{|link|
    numFound = 0
    #puts "Checking: #{link.linkID} => (#{link.upNode},#{link.downNode})"
    unless link.upNode.include? ':'
      #puts " up:"
      if nodes.has_key? link.upNode and nodeConv.has_key? link.upNode
        knownNodeIDs.push(link.upNode)
        numFound += 1
        #puts " +1A"
      end
    end
    unless link.downNode.include? ':'
      #puts " down:"
      if nodes.has_key? link.downNode and nodeConv.has_key? link.downNode
        knownNodeIDs.push(link.downNode)
        numFound += 1
        #puts " +1B"
      end
    end

    #Do we know about this link?
    puts "Found: #{link.upNode} => #{link.downNode}" if numFound==2
  }
  File.open('output_network.txt', 'w') {|f|
#    knownNodeIDs.uniq.each{|nodeID|
    allNodeIDs.each{|nodeID|
      nd = nodes[nodeID]
      next if nd.x==0 and nd.y==0
      name = 'multi'
      name = 'uni' if nodeID.include? ':'
      f.write("(\"#{name}-node\", 0, #{fakeNodeID(nodeID)}, {")  #Header
      f.write("\"xPos\":\"#{(nd.x*100).to_i}\",\"yPos\":\"#{(nd.y*100).to_i}\",") #Guaranteed
      f.write("\"mitsim-id\":\"#{nodeID}\",") if nodes.has_key? nodeID  #Optional (now it's guaranteed though)
      f.write("\"aimsun-id\":\"#{nodeConv[nodeID].nodeID}\",") if nodeConv.has_key? nodeID  #Optional
      f.write("})\n") #Footer
    }

    #Now write all Links
    possibleLinks.each{|link|
      f.write("(\"link\", 0, #{link.linkID}, {")  #Header
      f.write("\"road-name\":\"\",\"start-node\":\"#{fakeNodeID(link.upNode)}\",") #Guaranteed
      f.write("\"end-node\":\"#{fakeNodeID(link.downNode)}\",") #Also guaranteed
      f.write("\"fwd-path\":\"[")
      link.segments.each{|segment|
        f.write("#{segment.segmentID},")
      }
      f.write("]\",") #Close fwd-path
      f.write("\"rev-path\":\"[")
      #TODO: Currently there seems to be no way to do this.
      f.write("]\",") #Close rev-path
      f.write("})\n") #Footer
    }

    #Now write all Segments
    possibleLinks.each{|link|
      link.segments.each{|segment|
        f.write("(\"road-segment\", 0, #{segment.segmentID}, {")  #Header
        f.write("\"parent-link\":\"#{link.linkID}\",") #Guaranteed
        f.write("\"max-speed\":\"0\",") #Not hooked up yet
        f.write("\"lanes\":\"1\",") #Not hooked up yet
        f.write("\"from-node\":\"#{fakeNodeID(segment.upNode)}\",") #Not hooked up yet
        f.write("\"to-node\":\"#{fakeNodeID(segment.downNode)}\",") #Not hooked up yet
        f.write("})\n") #Footer
      }
    }
  }

  #Print the Agents
  File.open('agents.gen.xml', 'w') {|f|
    #Regular
    f.write("<drivers>\n") 
    drivers.keys.sort.each{|id|
      dr = drivers[id]
      next if (dr.originNode.x==0 and dr.originNode.y==0) or (dr.destNode.x==0 and dr.destNode.y==0)

      f.write("  <driver id='#{id}'")
      f.write(" originPos='(#{dr.originNode.x},#{dr.originNode.y})'")
      f.write(" destPos='(#{dr.destNode.x},#{dr.destNode.y})'")
      f.write(" time='#{dr.departure}'/>\n")
    }
    f.write("</drivers>\n") 

    #Skipped
    f.write("<drivers_SKIPPED>\n") 
    drivers.keys.sort.each{|id|
      dr = drivers[id]
      next unless (dr.originNode.x==0 and dr.originNode.y==0) or (dr.destNode.x==0 and dr.destNode.y==0)

      f.write("  <driver id='#{id}'")
      f.write(" originPos='(#{dr.originNode.x},#{dr.originNode.y})'")
      f.write(" destPos='(#{dr.destNode.x},#{dr.destNode.y})'")
      f.write(" time='#{dr.departure}'/>\n")
    }
    f.write("</drivers_SKIPPED>") 
  }


  #Some drivers are never started
  drvSkip = 0
  drivers.each {|id, dr|
    drvSkip += 1 unless dr.firstPos
  }

  #Print some statistics
  printf("%-13s %5s\n", 'Min agent id:', min.agentID)
  printf("%-13s %5s\n", 'Max agent id:', max.agentID)
  printf("%-13s %5s\n", 'Num agents:', drivers.length)
  raise "Agent IDs must be monotonic increasing from 1..num_agents" unless min.agentID==1 and max.agentID==drivers.length
  if drvSkip>0
    printf("A total of %d Drivers never started driving (%.2f%%)\n", drvSkip, (100.0*drvSkip/drivers.length))
  end
  puts 'Agents saved to agents.gen.xml'


end




if __FILE__ == $PROGRAM_NAME
  run_main()
end

