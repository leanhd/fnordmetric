class FnordMetric::Namespace
  
  attr_reader :handlers, :gauges, :opts, :key

  @@opts = [:event, :gauge]

  def initialize(key, opts)    
    @gauges = Hash.new
    @handlers = Hash.new([])         
    @opts = opts
    @key = key  
  end

  def ready!(redis)
    @redis = redis
  end

  def announce(event)                  

    if event[:_session]      
      FnordMetric::Session.create(@opts.clone.merge(
        :namespace_key => @key, 
        :namespace_prefix => key_prefix,
        :redis => @redis,
        :event => event
      ))        
    end

    @handlers[event[:_type]].each do |context| 
      context.call(event, @redis) 
    end

    self
  end

  def key_prefix
    [@opts[:redis_prefix], @key].join("-")
  end

  def method_missing(m, *args, &block)
    raise "unknown option: #{m}" unless @@opts.include?(m)
    send(:"opt_#{m}", *args, &block)
  end

  def opt_event(event_type, opts={}, &block)    
    opts.merge!(:redis => @redis, :gauges => @gauges)   
    FnordMetric::Context.new(opts, block).tap do |context|
      @handlers[event_type.to_s] << context
    end      
  end

  def opt_gauge(gauge_key, opts={})
    opts.merge!(:key => gauge_key, :key_prefix => key_prefix)
    @gauges[gauge_key] = FnordMetric::Gauge.new(opts)   
  end
    
end
