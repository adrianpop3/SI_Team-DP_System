package dp.system.back.controllers;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.integration.support.MessageBuilder;
import org.springframework.messaging.Message;
import org.springframework.messaging.MessageChannel;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Controller;

@Controller
@EnableScheduling
public class MqttController {
    @Autowired
    private MessageChannel mqttOutputChannel;
    @Scheduled(fixedDelay = 400) // send message every second
    public void request_reservation() {
        String payload = "{\n" +
                "\t“type”: “req_space”,\n" +
                "\t“seq_nr”: “1234”\n" +
                "}";
        Message<String> message = MessageBuilder.withPayload(payload).build();
        mqttOutputChannel.send(message);
    }

    @Scheduled(fixedDelay = 500) // send message every second
    public void reply_car_access() {
        String payload = "{\n" +
                "\t“type”: “reply_access”,\n" +
                "\t“license”: “TM12ABC”,\n" +
                "\t“verdict”: “Y/N/A1/B2”\n" +
                "}";
        Message<String> message = MessageBuilder.withPayload(payload).build();
        mqttOutputChannel.send(message);
    }
}
