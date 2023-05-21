package dp.system.back;

import dp.system.back.exceptions.ReservationNotFoundException;
import dp.system.back.exceptions.UserNotFoundException;
import dp.system.back.models.ParkingLot;
import dp.system.back.services.UserService;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.json.JSONException;
import org.json.JSONObject;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;
import org.springframework.integration.annotation.ServiceActivator;
import org.springframework.integration.channel.DirectChannel;
import org.springframework.integration.channel.QueueChannel;
import org.springframework.integration.core.MessageProducer;
import org.springframework.integration.mqtt.core.DefaultMqttPahoClientFactory;
import org.springframework.integration.mqtt.core.MqttPahoClientFactory;
import org.springframework.integration.mqtt.inbound.MqttPahoMessageDrivenChannelAdapter;
import org.springframework.integration.mqtt.outbound.MqttPahoMessageHandler;
import org.springframework.integration.mqtt.support.DefaultPahoMessageConverter;
import org.springframework.integration.support.MessageBuilder;
import org.springframework.messaging.MessageChannel;
import org.springframework.messaging.MessageHandler;
import org.springframework.messaging.PollableChannel;

import java.util.ArrayList;
import java.util.List;

@Configuration
@ComponentScan(basePackages = "dp.system.back.controllers")
public class MqttBeans {

    private final UserService userService;

    public MqttBeans(UserService userService) {
        this.userService = userService;
    }

    @Bean
    public MqttPahoClientFactory mqttClientFactory() {

        DefaultMqttPahoClientFactory factory = new DefaultMqttPahoClientFactory();
        MqttConnectOptions options = new MqttConnectOptions();

        options.setServerURIs(new String[]{"tcp://3.124.242.194:1883"});
        options.setCleanSession(false);
        factory.setConnectionOptions(options);

        return factory;
    }

    @Bean
    public PollableChannel mqttInputChannel() {
        return new QueueChannel();
    }

    @Bean
    public MessageProducer inbound() {
        MqttPahoMessageDrivenChannelAdapter adapter =
                new MqttPahoMessageDrivenChannelAdapter("springApplicationReceiver", mqttClientFactory(), "dpsystem");
        adapter.setCompletionTimeout(5000);
        adapter.setConverter(new DefaultPahoMessageConverter());
        adapter.setQos(2);
        adapter.setOutputChannel(mqttInputChannel());
        return adapter;
    }


    @Bean
    @ServiceActivator(inputChannel = "mqttInputChannel")
    public MessageHandler handler(MessageChannel mqttOutputChannel) {
        return message -> {
            try {
                String messageType, plateNumber;

                JSONObject json = new JSONObject(message.getPayload().toString());
                messageType = json.getString("type");
                JSONObject json_ans = new JSONObject();
                switch (messageType) {
                    case "reset_request" -> {
                        userService.resetReservations();
                        json_ans.put("type", "reset_reply");
                        mqttOutputChannel.send(MessageBuilder.withPayload(json_ans.toString()).build());
                    }
                    case "enter_request" -> {
                        plateNumber = json.getString("plateNumber");
                        json_ans.put("type", "enter_reply");
                        json_ans.put("plateNumber", plateNumber);
                        try {
                            String reservedParkingSpaceNumber = userService.findReservationByPlateNumber(plateNumber);
                            json_ans.put("parkingSpaceNumber", reservedParkingSpaceNumber);
                            mqttOutputChannel.send(MessageBuilder.withPayload(json_ans.toString()).build());
                        } catch (UserNotFoundException e) {
                            json_ans.put("parkingSpaceNumber", "N");
                            mqttOutputChannel.send(MessageBuilder.withPayload(json_ans.toString()).build());
                        } catch (ReservationNotFoundException e) {
                            json_ans.put("parkingSpaceNumber", "Y");
                            mqttOutputChannel.send(MessageBuilder.withPayload(json_ans.toString()).build());
                        }
                    }
                    case "exit_request" -> {
                        plateNumber = json.getString("plateNumber");
                        userService.deleteReservationForPlateNumber(plateNumber);
                        json_ans.put("type", "exit_reply");
                        json_ans.put("plateNumber", plateNumber);
                        mqttOutputChannel.send(MessageBuilder.withPayload(json_ans.toString()).build());
                    }
                    case "led_state" -> {
                        List<Integer> values = transformToInt(json.getString("values"));
                        ParkingLot parkingLot = new ParkingLot(values);
                        System.out.println(parkingLot);
                    }
                    default -> {
                    }
                }
            } catch (JSONException e) {
                System.out.println("JSONEXCEPTION");
            }
        };
    }

    private List<Integer> transformToInt(String input) {
        String[] elements = input.substring(1, input.length() - 1).split(",");

        List<Integer> arrayList = new ArrayList<>();
        for (String element : elements) {
            int value = Integer.parseInt(element.trim());
            arrayList.add(value);
        }
        return arrayList;
    }

    @Bean
    public MessageChannel mqttOutputChannel() {
        return new DirectChannel();
    }

    @Bean
    @ServiceActivator(inputChannel = "mqttOutputChannel")
    public MessageHandler outbound() {
        MqttPahoMessageHandler messageHandler = new MqttPahoMessageHandler("springApplicationSender", mqttClientFactory());
        messageHandler.setAsync(true);
        messageHandler.setDefaultTopic("dpsystem");
        messageHandler.setDefaultRetained(false);
        return messageHandler;
    }
}
